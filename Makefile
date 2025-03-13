all : main

tests: test_hashmap test_parser test_memoryHandler

test_hashmap : hashmap.o tests/test_hashmap.c
	gcc -Wall -c tests/test_hashmap.c -ggdb -o tests/test_hashmap.o
	gcc -Wall tests/test_hashmap.o hashmap.o -ggdb -o test_hashmap

test_parser: parser.o tests/test_parser.c
	gcc -Wall -c tests/test_parser.c -ggdb -o tests/test_parser.o
	gcc -Wall hashmap.o parser.o tests/test_parser.o -ggdb -o test_parser

test_memoryHandler : hashmap.o memoryHandler.o tests/test_memoryHandler.c
	gcc -Wall -c tests/test_memoryHandler.c -ggdb -o tests/test_memoryHandler.o
	gcc -Wall hashmap.o memoryHandler.o tests/test_memoryHandler.o -ggdb -o test_memoryHandler

main : hashmap.o main.c
	gcc -Wall -c main.c -ggdb -o main.o
	gcc -Wall hashmap.o main.o -ggdb -o main

parser.o: hashmap.o parser.c parser.h
	gcc -Wall -c parser.c -ggdb -o parser.o

memoryHandler.o : hashmap.o memoryHandler.c memoryHandler.h
	gcc -Wall -c memoryHandler.c -ggdb -o memoryHandler.o

hashmap.o : hashmap.c hashmap.h
	gcc -Wall -c hashmap.c -ggdb -o hashmap.o

clean:
	rm -rf *.o tests/*.o main main_hashmap test_hashmap test_memoryHandler test_parser
