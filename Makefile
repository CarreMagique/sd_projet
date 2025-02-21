all : main

test : hashmap.o tests/tests.c
	gcc -Wall -c tests/tests.c -ggdb -o tests/tests.o
	gcc -Wall tests/tests.o hashmap.o -ggdb -o test
main_hashmap: hashmap.o main_hashmap.c
	gcc -Wall -c main_hashmap.c -ggdb -o main_hashmap.o
	gcc -Wall hashmap.o main_hashmap.o -ggdb -o main_hashmap

main : hashmap.o main.c
	gcc -Wall -c main.c -ggdb -o main.o
	gcc -Wall hashmap.o main.o -ggdb -o main

main2 : hashmap.o memoryHandler.o main2.c
	gcc -Wall -c main2.c -ggdb -o main2.o
	gcc -Wall hashmap.o memoryHandler.o main2.o -ggdb -o main2

memoryHandler.o : hashmap.o memoryHandler.c memoryHandler.h
	gcc -Wall -c memoryHandler.c -ggdb memoryHandler.o

hashmap.o : hashmap.c hashmap.h
	gcc -Wall -c hashmap.c -ggdb -o hashmap.o
clean:
	rm -rf *.o main
