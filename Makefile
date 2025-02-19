all : main

main : hashmap.o main.c
	gcc -Wall -c main.c -ggdb -o main.o
	gcc -Wall hashmap.o main.o -ggdb -o main

memoryHandler.o : hashmap.o memoryHandler.c memoryHandler.h
	gcc -Wall -c memoryHandler.c -ggdb memoryHandlerFirst.o
	gcc -Wall -c memoryHandlerFirst.o hashmap.o -ggdb -o memoryHandler.o

hashmap.o : hashmap.c hashmap.h
	gcc -Wall -c hashmap.c -ggdb -o hashmap.o
clean:
	rm -rf *.o main