CC=gcc -Wall -g
ALLOC=allocator

build: allocator.c main.c
	$(CC) $^ -o $(ALLOC)
	
clean:
	rm -f $(ALLOC)

