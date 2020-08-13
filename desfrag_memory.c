#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "memory.h"

// --- Global variables
chunkStatus *head = NULL;
chunkStatus *lastVisited = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *brkPoint0 = NULL;

/* findChunk: busca el primer fragmento que se ajuste (tamaño igual o mayor) a la solicitud
               del usuario.
      chunkStatus * headptr: puntero al primer bloque de memoria en el montón
      unsigned int size: tamaño solicitado por el usuario
      retval: un indicador del bloque que se ajusta a la solicitud
o NULL, en caso de que no exista tal bloque en la lista
*/
chunkStatus* findChunk(chunkStatus *headptr, unsigned int size)
{
  chunkStatus* ptr = headptr;
  
  while(ptr != NULL)
  {
    if(ptr->size >= (size + STRUCT_SIZE) && ptr->available == 1)
    {
      return ptr;
    }
    lastVisited = ptr;
    ptr = ptr->next;
  }  
  return ptr;  
}

/* splitChunk: divide un bloque grande en dos. El primero tendrá el tamaño solicitado por el usuario.
   el segundo tendrá el resto.
      chunkStatus * ptr: puntero al bloque de memoria que se va a dividir.
      unsigned int size: tamaño solicitado por el usuario
      retval: void, la función modifica la lista
*/

void splitChunk(chunkStatus* ptr, unsigned int size)
{
  chunkStatus *newChunk;	
  
  newChunk = ptr->end + size;
  newChunk->size = ptr->size - size - STRUCT_SIZE;
  newChunk->available = 1;
  newChunk->next = ptr->next;
  newChunk->prev = ptr;
  
   if((newChunk->next) != NULL)
   {      
      (newChunk->next)->prev = newChunk;
   }
  
  ptr->size = size;
  ptr->available = 0;
  ptr->next = newChunk;
}

/* inscreaseAllocation: aumenta la cantidad de memoria disponible en el montón, cambiando su punto de interrupción
      chunkStatus * ptr: puntero al bloque de memoria que se va a dividir.
      unsigned int size: tamaño solicitado por el usuario
      retval: void, la función modifica la lista
*/
chunkStatus* increaseAllocation(chunkStatus *lastVisitedPtr, unsigned int size)
{
  brkPoint0 = sbrk(0);
  chunkStatus* curBreak = brkPoint0;		//Current breakpoint of the heap
  
  if(sbrk(MULTIPLIER * (size + STRUCT_SIZE)) == (void*) -1)
  {
    return NULL;
  }
  
  curBreak->size = (MULTIPLIER * (size + STRUCT_SIZE)) - STRUCT_SIZE;
  curBreak->available = 0;
  curBreak->next = NULL;
  curBreak->prev = lastVisitedPtr;
  lastVisitedPtr->next = curBreak;
  
  if(curBreak->size > size)
    splitChunk(curBreak, size);
  
  return curBreak;  
}


/* mergeChunkPrev: fusiona un fragmento liberado con su predecesor (en caso de que también sea gratis)
      chunkStatus * liberado: puntero al bloque de memoria que se va a liberar.
      retval: void, la función modifica la lista
*/
void mergeChunkPrev(chunkStatus *freed)
{ 
  chunkStatus *prev;
  prev = freed->prev;
  
  if(prev != NULL && prev->available == 1)
  {
    prev->size = prev->size + freed->size + STRUCT_SIZE;
    prev->next = freed->next;
    if( (freed->next) != NULL )
      (freed->next)->prev = prev;
  }
}

/* mergeChunkNext: fusiona un fragmento liberado con el siguiente fragmento (en caso de que también sea gratuito)
      chunkStatus * liberado: puntero al bloque de memoria que se va a liberar.
      retval: void, la función modifica la lista
*/
void mergeChunkNext(chunkStatus *freed)
{  
  chunkStatus *next;
  next = freed->next;
  
  if(next != NULL && next->available == 1)
  {
    freed->size = freed->size + STRUCT_SIZE + next->size;
    freed->next = next->next;
    if( (next->next) != NULL )
      (next->next)->prev = freed;
  }
}


/* printList: imprime la lista completa de Me gusta. Para fines de depuración
      chunkStatus * headptr: apunta al comienzo de la lista
      retval: void, solo imprime
*/
void printList(chunkStatus *headptr)
{
int i = 0;
  chunkStatus *p = headptr;
  
  while(p != NULL)
  {
    printf("[%d] p: %d\n", i, p);
    printf("[%d] p->size: %d\n", i, p->size);
    printf("[%d] p->available: %d\n", i, p->available);
    printf("[%d] p->prev: %d\n", i, p->prev);
    printf("[%d] p->next: %d\n", i, p->next);
    printf("__________________________________________________\n");
    i++;
    p = p->next;
  }
}

/* mymalloc: asigna memoria en el montón del tamaño solicitado. El bloque
              de la memoria devuelta siempre debe rellenarse para que comience
              y termina en un límite de palabras.
      unsigned int size: el número de bytes a asignar.
      retval: un puntero al bloque de memoria asignado o NULL si el
              no se pudo asignar memoria.
              (NOTA: el sistema también establece errno, pero no somos el sistema,
                     por lo que no está obligado a hacerlo).
*/
void *mymalloc(unsigned int _size) 
{
  //pthread_mutex_lock(&lock);
  
  
  void *brkPoint1;
  unsigned int size = ALIGN(_size);
  int memoryNeed = MULTIPLIER * (size + STRUCT_SIZE);
  chunkStatus *ptr;
  chunkStatus *newChunk;

  pthread_mutex_lock(&lock);
  brkPoint0 = sbrk(0);
  
  
  if(head == NULL)				//First time running: create free list
  {
    if(sbrk(memoryNeed) == (void*) -1)		//error check
    {
      pthread_mutex_unlock(&lock);
      return NULL;
    }
    
    //Create the first chunk with size equals all memory available in the heap after setting the new breakpoint
    brkPoint1 = sbrk(0);
    head = brkPoint0;
    head->size = memoryNeed - STRUCT_SIZE;
    head->available = 0;
    head->next = NULL;
    head->prev = NULL;
    
    //Split the chunk into two: one with size request by user, other with the remainder.
    ptr = head;
    
    //Verify if the split in the first allocation is necessary
    if(MULTIPLIER > 1)  
      splitChunk(ptr, size);

    pthread_mutex_unlock(&lock);
    
    return ptr->end;
  }
  
  else								//Not first time running
  {
    chunkStatus *freeChunk = NULL;
    freeChunk = findChunk(head, size);
    
    if(freeChunk == NULL)					//Didn't find any chunk available
    {
      freeChunk = increaseAllocation(lastVisited, size);	//Extend the heap
      if(freeChunk == NULL) 					//Couldn't extend heap. increaseAllocation returned NULL (sbrk error)
      {
	pthread_mutex_unlock(&lock);
	return NULL;
      }
      pthread_mutex_unlock(&lock);
      return freeChunk->end;
    }
    
    else						//A chunk was found
    {
      if(freeChunk->size > size)			//If chunk is too big, split it
	splitChunk(freeChunk, size);
    }    
    pthread_mutex_unlock(&lock);    
    return freeChunk->end;
  }  
}

/* myfree: desasigna la memoria que se ha asignado con mymalloc.
      void * ptr: puntero al primer byte de un bloque de memoria asignado por
                 mymalloc.
      retval: 0 si la memoria se liberó correctamente y 1 en caso contrario.
              (NOTA: la versión del sistema gratuita no devuelve ningún error).
*/
unsigned int myfree(void *ptr) {
	//#if SYSTEM_MALLOC
	/*free(ptr);
	return 0;
	#endif	
	return 1; */
	pthread_mutex_lock(&lock);
	
	chunkStatus *toFree;
	toFree = ptr - STRUCT_SIZE;
	
	if(toFree >= head && toFree <= brkPoint0)
	{
	  toFree->available = 1;	
	  mergeChunkNext(toFree);
	  mergeChunkPrev(toFree);
	  pthread_mutex_unlock(&lock);
	  return 0;
	  
	}
	else
	{
	  //#endif
	  pthread_mutex_unlock(&lock);
	  return 1;
	}
}