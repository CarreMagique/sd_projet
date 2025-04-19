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

int main() {
    FILE *f1, *f2, *f3, *f4;
    /*On initialise des variables FILE pour chaque fichiers de temps*/
    f1=fopen("out_hash.txt","w");
    f2=fopen("out_memory_handler.txt","w");
    f3=fopen("out_parser.txt","w");
    f4=fopen("out_cpu.txt","w");
    printf("Program initialized\n");
    for(int i=1; i<100; i++) {
        printf("i : %d\n", i);
        test_hash(f1, i*200);
        test_memory_handler(f2, i*200);
    }
    printf("Program ended\n");
    fclose(f1);
    fclose(f2);
    fclose(f3);
    fclose(f4);
    return 0;
}