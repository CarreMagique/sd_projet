#include "cpu.h"
#include "memoryHandler.h"

CPU *cpu_init(int memory_size) {
    CPU* cpu = (CPU *) malloc(sizeof(CPU));
    assert(cpu);
    cpu->memory_handler=memory_init(memory_size);
    cpu->constant_pool=hashmap_create();
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
    free(hashmap_get(cpu->context,"AX"));
    free(hashmap_get(cpu->context,"BX"));
    free(hashmap_get(cpu->context,"CX"));
    free(hashmap_get(cpu->context,"DX"));
    
    Segment *DS = hashmap_get(cpu->memory_handler->allocated, "DS");
    free_memory_handler(cpu->memory_handler, DS->start+DS->size); //On utilise la taille sinon seg fault
    free_segment(DS);
    hashmap_destroy(cpu->context);
    hashmap_destroy(cpu->constant_pool);
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
        if(cpu->memory_handler->memory[i]) {
            printf("%d\t",* (int *)(cpu->memory_handler->memory[i]));
        } else {
            printf(".\t");
        }
        
    }
    printf("\n");
}

int matches(const char * pattern, const char * string) {
    regex_t regex;
    int result = regcomp(&regex, pattern, REG_EXTENDED);
    if (result) {
        fprintf(stderr, "Regex compilation failed for pattern %s\n" , pattern) ;
        return 0;
    }
    result = regexec(&regex, string, 0, NULL, 0);
    regfree(&regex);
    return result == 0;
}

void *immediate_addressing(CPU *cpu, const char *operand) {
    if(matches("^[0-9]*$",operand)) {
        int *data=(int *)malloc(sizeof(int));
        sscanf(operand, "%d", data);
        if(hashmap_get(cpu->constant_pool, operand)==NULL) {
            hashmap_insert(cpu->constant_pool, operand, data);
        }
        return data;
    }
    return NULL;
}

void *register_addressing(CPU *cpu, const char *operand) {
    if(matches("^[A-D]X$",operand)) {

        int *data = hashmap_get(cpu->context, operand);
        return data;
    }
    return NULL;
}

void *memory_direct_addressing(CPU *cpu, const char *operand) {
    if(matches("^\\[[0-9]*\\]$",operand)) {
        int *data=(int *)malloc(sizeof(int));
        sscanf(operand, "[%d]", data);
        return cpu->memory_handler->memory[*data];
    }
    return NULL;
}

void *register_indirect_addressing(CPU *cpu, const char*operand) {
    if(matches("^\\[[A-D]X\\]$",operand)) {
        char buffer[3];
        sscanf(operand, "[%s]", buffer);
        buffer[2]='\0'; //sinon seg fault
        int* index = hashmap_get(cpu->context, buffer);
        if(index!=NULL) {
            return cpu->memory_handler->memory[*index];
        }        
        return NULL;
    }
    return NULL;
}

void handle_MOV(CPU* cpu, void* src, void* dest) {
    /*
    assert(* (int *)src>=0);
    assert(* (int *)dest>=0);
    assert(cpu->memory_handler->total_size> *(int *)src);
    assert(cpu->memory_handler->total_size> *(int *)dest);
    cpu->memory_handler->memory[* (int *)dest]=cpu->memory_handler->memory[* (int *)src];
    */
    *(int *)dest=*(int *)src;
}

void *resolve_addressing(CPU *cpu, const char *operand){
    void* res = immediate_addressing(cpu, operand);
    if(res){
        return res;
    }
    res = register_indirect_addressing(cpu,operand);
    if(res){
        return res;
    }
    res = memory_direct_addressing(cpu, operand);
    if(res){
        return res;
    }
    res = register_addressing(cpu, operand) ;
    if(res){
        return res;
    }
    return NULL;
}
