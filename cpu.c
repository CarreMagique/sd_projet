#include "cpu.h"
#include "hashmap.h"
#include "memoryHandler.h"

CPU *cpu_init(int memory_size) {
    CPU* cpu = (CPU *) malloc(sizeof(CPU));
    assert(cpu);
    cpu->memory_handler=memory_init(memory_size);
    cpu->context=hashmap_create();
    int* a=malloc(sizeof(int));
    *a=0;
    hashmap_insert(cpu->context, "AX", a);
    int* b=malloc(sizeof(int));
    *b=0;
    hashmap_insert(cpu->context, "BX", b);
    int* c=malloc(sizeof(int));
    *c=0;
    hashmap_insert(cpu->context, "CX", c);
    int* d=malloc(sizeof(int));
    *d=0;
    hashmap_insert(cpu->context, "DX", d);
    
    create_segment(cpu->memory_handler, "espace_plaisir", 0, 100);
    return cpu;
}

void cpu_destroy(CPU *cpu) {
    Segment *parcours=cpu->memory_handler->free_list;
    Segment *temp;
    while(parcours) {
        temp=parcours;
        parcours=parcours->next;
        free(temp);
    }
    free(cpu->memory_handler->memory);
    hashmap_destroy(cpu->memory_handler->allocated);
    free(cpu->memory_handler);
    hashmap_destroy(cpu->context);
    free(cpu);
}

void* load(MemoryHandler *handler, const char *segment_name, int pos){
    Segment* smg = hashmap_get(handler->allocated, segment_name);
    if(!smg){
        return NULL;
    }
    if(smg->size<pos){
        return TOMBSTONE;
    }
    void* data = handler->memory[smg->start+pos];
    return data;
}

void* store(MemoryHandler *handler, const char *segment_name,int pos, void *data) {
    Segment *seg = hashmap_get(handler->allocated, segment_name);
    if(seg==NULL) {return NULL;}
    if(pos>seg->size) {return NULL;}

    handler->memory[pos+seg->start]=data;

    return data;
}

void print_data_segment(CPU *cpu) {
    Segment *DS = hashmap_get(cpu->memory_handler->allocated, "DS");
    for(int i=DS->start; i <DS->start+DS->size; i++) {
        printf("%d\t",* (int *)(cpu->memory_handler->memory[i]));
    }
    printf("\n");
}