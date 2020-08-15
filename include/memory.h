//Defines and macros
#define SYSTEM_MALLOC 1
#define STRUCT_SIZE 24 
#define MULTIPLIER 10
#define ALIGN_SIZE 8
#define ALIGN(size) (((size) + (ALIGN_SIZE-1)) & ~(ALIGN_SIZE-1))

// --- Struct to store memory's block metadata
typedef struct blockStatus
{
  int size;
  int available;
  struct blockStatus* next;
  struct blockStatus* prev;
  char end[1]; 		//end represents the end of the metadata struct
} blockStatus;


blockStatus* buscarBlock(blockStatus *headptr, unsigned int size);

void dividirBlock(blockStatus* ptr, unsigned int size);

blockStatus* incrementAllocation(blockStatus *tailptr, unsigned int size);

void combinarBlock(blockStatus *freed);

void printList(blockStatus *headptr);

void *mymalloc(unsigned int _size);

unsigned int myfree(void *ptr);