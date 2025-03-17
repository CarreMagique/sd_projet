#include "CPU.h"
#include "hashmap.h"
#include "memoryHandler.h"

CPU *cpu_init(int memory_size) {
    CPU* cpu = (CPU *) malloc(sizeof(CPU));
    assert(cpu);
    cpu->memory_handler=memory_init(memory_size);
    cpu->context=hashmap_create();
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

