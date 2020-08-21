


bin/main: obj/memory_manager.o  obj/main.o
	gcc obj/memory_manager.o obj/main.o -o bin/memory_manager

obj/main.o: src/main.c
	gcc -Wall -c -I include/ src/main.c -o obj/main.o

obj/memory_manager.o: src/memory_manager.c
	gcc -Wall -c src/memory_manager.c -o obj/memory_manager.o



clean:
	rm obj/*.o bin/memory_manager