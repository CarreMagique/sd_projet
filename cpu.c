#include "cpu.h"
#include "memoryHandler.h"
#include "parser.h"

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
    void *data=hashmap_get(cpu->context,"AX");
    if(data) {free(data);}

    data=hashmap_get(cpu->context,"BX");
    if(data) {free(data);}

    data=hashmap_get(cpu->context,"CX");
    if(data) {free(data);}

    data=hashmap_get(cpu->context,"DX");
    if(data) {free(data);}

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
//Rajouter tests store
void allocate_variables(CPU *cpu, Instruction** data_instructions, int data_count) {
    Instruction *ins;
    int size = 0;
    for(int i=0; i<data_count; i++) {
        ins=data_instructions[i];
        //ça c'est faux c'est juste que l'array s'appelle arr, enfaite il n'y aucun moyen de distinguer une array d'une variable classique, faudra demander à Anissa
        for(int j = 0; ins->operand2[j]!='\0'; j++){
            if(ins->operand2[j] == ','){
                size = size+1;
            }
        }
        size = size+1;
    }
    //On peut pas faire une seule boucle car il faut connaitre la taille du segment avant de l'initialiser
    int start = 0;
    int res = create_segment(cpu->memory_handler, "DS", start, size);
    if(res == 1){
        printf("Problème");
    }
    int c_m = start;
    for(int i=0; i<data_count; i++) {
        ins=data_instructions[i];
        //On modifiera ça après
        char buffer[100];
        int b = 0;
        for(int j = 0; ins->operand2[j]!='\0'; j++){
            buffer[b] = ins->operand2[j];
            b = b +1;
            if(ins->operand2[j] == ','){
                int* p = malloc(sizeof(int));
                *p= atoi(buffer);
                store(cpu->memory_handler, "DS", c_m, (void*) p);
                c_m = c_m+1;
                b = 0;
            }
        }
        int* p = malloc(sizeof(int));
        *p= atoi(buffer);
        store(cpu->memory_handler, "DS", c_m, (void*) p);
        c_m = c_m+1;
    }
}

void print_data_segment(CPU *cpu) {
    Segment *DS = hashmap_get(cpu->memory_handler->allocated, "DS");
    for(int i=DS->start; i <DS->start+DS->size; i++) {
        printf("%d\t",* (int *)(cpu->memory_handler->memory[i]));
    }
    printf("\n");
}