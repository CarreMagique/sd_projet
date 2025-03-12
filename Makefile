all : main

test : hashmap.o tests/tests.c
	gcc -Wall -c tests/tests.c -ggdb -o tests/tests.o
	gcc -Wall tests/tests.o hashmap.o -ggdb -o test

tests: main_hashmap main_parser main_memoryHandler

main_hashmap: hashmap.o main_hashmap.c
	gcc -Wall -c main_hashmap.c -ggdb -o main_hashmap.o
	gcc -Wall hashmap.o main_hashmap.o -ggdb -o main_hashmap

main_parser: parser.o main_parser.c
	gcc -Wall -c main_parser.c -ggdb -o main_parser.o
	gcc -Wall hashmap.o parser.o main_parser.o -ggdb -o main_parser

main : hashmap.o main.c
	gcc -Wall -c main.c -ggdb -o main.o
	gcc -Wall hashmap.o main.o -ggdb -o main

main_memoryHandler : hashmap.o memoryHandler.o main_memoryHandler.c
	gcc -Wall -c main_memoryHandler.c -ggdb -o main_memoryHandler.o
	gcc -Wall hashmap.o memoryHandler.o main_memoryHandler.o -ggdb -o main_memoryHandler

parser.o: hashmap.o parser.c parser.h
	gcc -Wall -c parser.c -ggdb -o parser.o

memoryHandler.o : hashmap.o memoryHandler.c memoryHandler.h
	gcc -Wall -c memoryHandler.c -ggdb -o memoryHandler.o

hashmap.o : hashmap.c hashmap.h
	gcc -Wall -c hashmap.c -ggdb -o hashmap.o

clean:
	rm -rf *.o main main_hashmap main_memoryHandler main_parser
