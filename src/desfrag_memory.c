#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "../include/memory.h"

// --- Global variables
blockStatus *head = NULL;
blockStatus *lastVisited = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *brkPoint0 = NULL;

/* findChunk: busca el primer fragmento que se ajuste (tamaño igual o mayor) a la solicitud
               del usuario.
      chunkStatus * headptr: puntero al primer bloque de memoria en el montón
      unsigned int size: tamaño solicitado por el usuario
      retval: un indicador del bloque que se ajusta a la solicitud
o NULL, en caso de que no exista tal bloque en la lista
*/
blockStatus* buscarBlock(blockStatus *headptr, unsigned int size)
{
  blockStatus* ptr = headptr;
  
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

void dividirBlock(blockStatus* ptr, unsigned int size)
{
  blockStatus *newBlock;	
  
  newBlock = ptr->end + size;
  newBlock->size = ptr->size - size - STRUCT_SIZE;
  newBlock->available = 1;
  newBlock->next = ptr->next;
  newBlock->prev = ptr;
  
   if((newBlock->next) != NULL)
   {      
      (newBlock->next)->prev = newBlock;
   }
  
  ptr->size = size;
  ptr->available = 0;
  ptr->next = newBlock;
}

/* inscreaseAllocation: aumenta la cantidad de memoria disponible en el montón, cambiando su punto de interrupción
      chunkStatus * ptr: puntero al bloque de memoria que se va a dividir.
      unsigned int size: tamaño solicitado por el usuario
      retval: void, la función modifica la lista
*/
blockStatus* incrementAllocation(blockStatus *lastVisitedPtr, unsigned int size)
{
  brkPoint0 = sbrk(0);
  blockStatus* curBreak = brkPoint0;		//Current breakpoint of the heap
  
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
    dividirBlock(curBreak, size);
  
  return curBreak;  
}


/* mergeChunkPrev: fusiona un fragmento liberado con su predecesor (en caso de que también sea gratis)
      chunkStatus * liberado: puntero al bloque de memoria que se va a liberar.
      retval: void, la función modifica la lista
*/
void combinarChunkPrev(blockStatus *freed)
{ 
  blockStatus *prev;
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
void combinarChunkNext(blockStatus *freed)
{  
  blockStatus *next;
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
void printList(blockStatus *headptr)
{
int i = 0;
  blockStatus *p = headptr;
  
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
  blockStatus *ptr;
  blockStatus *newChunk;

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
      dividirBlock(ptr, size);

    pthread_mutex_unlock(&lock);
    
    return ptr->end;
  }
  
  else								//Not first time running
  {
    blockStatus *freeBlock = NULL;
    freeBlock = buscarBlock(head, size);
    
    if(freeBlock == NULL)					//Didn't find any chunk available
    {
      freeBlock = incrementAllocation(lastVisited, size);	//Extend the heap
      if(freeBlock == NULL) 					//Couldn't extend heap. increaseAllocation returned NULL (sbrk error)
      {
	pthread_mutex_unlock(&lock);
	return NULL;
      }
      pthread_mutex_unlock(&lock);
      return freeBlock->end;
    }
    
    else						//A chunk was found
    {
      if(freeBlock->size > size)			//If chunk is too big, split it
	dividirBlock(freeBlock, size);
    }    
    pthread_mutex_unlock(&lock);    
    return freeBlock->end;
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
	
	blockStatus *toFree;
	toFree = ptr - STRUCT_SIZE;
	
	if(toFree >= head && toFree <= brkPoint0)
	{
	  toFree->available = 1;	
	  combinarChunkNext(toFree);
	  combinarChunkPrev(toFree);
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