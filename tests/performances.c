#include <bits/types/clock_t.h>
#include <time.h>
#include <stdio.h>
#include "../hashmap.h"
#include "../memoryHandler.h"
#include "../parser.h"
#include "../cpu.h"

#define FICHIER_TEST "GdeBiblio.txt"

void test_hash(FILE *f, int n) {
    HashMap* map = hashmap_create();
    char buffer[n];
    for(int i=0; i<n-1; i++) {
        buffer[i]='a';
    }
    buffer[n-1]='\0';

    clock_t init_insert=clock();
    for(int j=0; j<n; j++) {
        int *val=malloc(sizeof(int));
        *val=1;
        buffer[j]='b';
        hashmap_insert(map, buffer, (void *)val);
    }
    clock_t end_insert=clock();

    for(int i=0; i<n-1; i++) {
        buffer[i]='a';
    }
    buffer[n-1]='\0';

    clock_t init_get=clock();
    for(int j=0; j<n; j++) {
        buffer[j]='b';
        hashmap_get(map, buffer);
    }
    clock_t end_get=clock();

    for(int i=0; i<n-1; i++) {
        buffer[i]='a';
    }
    buffer[n-1]='\0';

    clock_t init_remove=clock();
    for(int j=0; j<n; j++) {
        buffer[j]='b';
        hashmap_remove(map, buffer);
    }
    clock_t end_remove=clock();
    fprintf(f, "%d %.6f %.6f %.6f\n", n, (double) (end_insert-init_insert)/CLOCKS_PER_SEC, (double) (end_get-init_get)/CLOCKS_PER_SEC, (double) (end_remove-init_remove)/CLOCKS_PER_SEC);

    hashmap_destroy(map);
}

void test_memory_handler(FILE *f, int n) {
    MemoryHandler* mem=memory_init(n);
    char name[n];
    for(int i=0; i<n-1; i++) {
        name[i]='a';
    }
    name[n-1]='\0';
    clock_t init_create=clock();
    for(int j=1; j<mem->total_size; j++) {
        name[j]='b';
        create_segment(mem, name, j, 1);
    }
    clock_t end_create=clock();

    for(int i=0; i<n-1; i++) {
        name[i]='a';
    }
    name[n-1]='\0';
    
    clock_t init_remove=clock();
    for(int j=1; j<mem->total_size; j++) {
        name[j]='b';
        remove_segment(mem, name);
    }
    clock_t end_remove=clock();
    
    
    fprintf(f, "%d %.6f %.6f\n", n, (double) (end_create-init_create)/CLOCKS_PER_SEC, (double) (end_remove-init_remove)/CLOCKS_PER_SEC);
    free_memory_handler(mem);
}

void test_parser_and_cpu(FILE *f_parser, FILE *f_cpu, const char * file_input_name, int n) {
    clock_t init_parse = clock();
    ParserResult *pr = parse(file_input_name);
    clock_t end_parse = clock();

    CPU *cpu = cpu_init(2*(n+128)); //+128 car si n<128, alors on ne peut pas allouer de Stack Segment
    clock_t init_allocate = clock();
    allocate_variables(cpu, pr->data_instructions, pr->data_count);
    clock_t end_allocate = clock();

    clock_t init_free_parser = clock();
    free_parser_result(pr);
    clock_t end_free_parser = clock();

    clock_t init_free_cpu = clock();
    cpu_destroy(cpu);
    clock_t end_free_cpu = clock();

    fprintf(f_parser, "%d %.6f %.6f\n", n, (double) (end_parse-init_parse)/CLOCKS_PER_SEC, (double) (end_free_parser-init_free_parser)/CLOCKS_PER_SEC);

    fprintf(f_cpu, "%d %.6f %.6f\n", n, (double) (end_allocate-init_allocate)/CLOCKS_PER_SEC, (double) (end_free_cpu-init_free_cpu)/CLOCKS_PER_SEC);
}

int main() {
    FILE *f1, *f2, *f3, *f4;
    /*On initialise des variables FILE pour chaque fichiers de temps*/
    f1=fopen("out_hash.txt","w");
    f2=fopen("out_memory_handler.txt","w");
    f3=fopen("out_parser.txt","w");
    f4=fopen("out_cpu.txt","w");
    printf("Program initialized\n");
    for(int i=1; i<11; i++) {
        printf("Value : %d\n", i*2000);
        test_hash(f1, i*2000);
        test_memory_handler(f2, i*2000);
    }
    
    //Le dernier parametre est le nombre de lignes maximal entre la partie .DATA et .CODE
    test_parser_and_cpu(f3, f4, "tests/test_parser.txt", 3);
    test_parser_and_cpu(f3, f4, "tests/perf_parser1000.txt", 1152);
    test_parser_and_cpu(f3, f4, "tests/perf_parser2000.txt", 2304);
    test_parser_and_cpu(f3, f4, "tests/perf_parser4000.txt", 4608);
    test_parser_and_cpu(f3, f4, "tests/perf_parser8000.txt", 9216);
    test_parser_and_cpu(f3, f4, "tests/perf_parser16000.txt", 18432);
    printf("Program ended\n");
    fclose(f1);
    fclose(f2);
    fclose(f3);
    fclose(f4);
    return 0;
}