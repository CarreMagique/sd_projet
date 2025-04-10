all : main

tests: test_hashmap test_parser test_memoryHandler test_cpu test_cpu2

test_cpu : cpu.o tests/test_cpu.c
	gcc -Wall -c tests/test_cpu.c -ggdb -o tests/test_cpu.o
	gcc -Wall tests/test_cpu.o cpu.o parser.o hashmap.o memoryHandler.o -ggdb -o test_cpu

test_cpu2 : cpu.o tests/test_cpu2.c
	gcc -Wall -c tests/test_cpu2.c -ggdb -o tests/test_cpu2.o
	gcc -Wall tests/test_cpu2.o cpu.o parser.o hashmap.o memoryHandler.o -ggdb -o test_cpu2

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

cpu.o : cpu.c cpu.h memoryHandler.o hashmap.o parser.o
	gcc -Wall -c cpu.c -ggdb -o cpu.o

clean:
	rm -rf *.o tests/*.o main main_hashmap test_hashmap test_memoryHandler test_parser test_cpu
