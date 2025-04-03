#include "cpu.h"

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

    int* y = malloc(sizeof(int));
    hashmap_insert(cpu->context, "DB", y);

    int* i=malloc(sizeof(int));
    *i=0;
    hashmap_insert(cpu->context, "IP", i);
    int* s=malloc(sizeof(int));
    *s=0;
    hashmap_insert(cpu->context, "SF", s);
    int* z=malloc(sizeof(int));
    *z=0;
    hashmap_insert(cpu->context, "ZF", z);
    
    return cpu;
}

void cpu_destroy(CPU *cpu) {
    Segment *DS = hashmap_get(cpu->memory_handler->allocated, "DS");
    free_memory_handler(cpu->memory_handler, DS->start+DS->size); //On utilise la taille sinon seg fault
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
    if(pos>seg->size || pos>handler->total_size) {return NULL;}
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
        return;
    }
    int c_m = start;
    for(int i=0; i<data_count; i++) {
        ins=data_instructions[i];
        //On modifiera ça après
        char buffer[100];
        int b = 0;
        for(int j = 0; ins->operand2[j]!='\0'; j++){
            buffer[b] = ins->operand2[j];
            b++;
            if(ins->operand2[j] == ','){
                int* p = malloc(sizeof(int));
                *p= atoi(buffer);
                store(cpu->memory_handler, "DS", c_m, (void*) p);
                c_m++;
                b = 0;
            }
        }
        int* p = malloc(sizeof(int));
        *p= atoi(buffer);
        store(cpu->memory_handler, "DS", c_m, (void*) p);
        c_m++;
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
        sscanf(operand, " %d ", data);
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
        int data=0;
        sscanf(operand, " %d ", &data);
        return cpu->memory_handler->memory[data];
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

void handle_ADD(CPU* cpu, void* src, void* dest){
    *(int *)dest=*(int *)src+*(int *)dest;
}

int handle_CMP(CPU* cpu, void* src, void* dest){
    int v_dest = *(int *)dest;
    int v_src = *(int *)src;
    if(v_dest == v_src){
        int* zf = (int *)hashmap_get(cpu->context, "ZF");
        *zf = 1;
        return 0;
    }
    if(v_dest < v_src){
        int* sf = (int *)hashmap_get(cpu->context, "SF");
        *sf = 1;
        return 0;
    }
    return 1;
}

int handle_JMP(CPU* cpu, void* src, void* dest){
    int * ip = (int *)hashmap_get(cpu->context, "IP");
    if(ip==NULL) {
        return 1;
    }
    *ip = *(int*)dest;
    return 0;
}

int handle_JZ(CPU* cpu, void* src, void* dest){
    int * zf = (int *)hashmap_get(cpu->context, "ZF");
    if(zf==NULL) {
        return 1;
    }
    if(*zf == 1){
        int * ip = (int *)hashmap_get(cpu->context, "IP");
        if(ip==NULL) {
            return 1;
        }
        *ip = *(int *) dest;
        return 0;
    }
    return 1;
}

int handle_JNZ(CPU* cpu, void* src, void* dest){
    int *zf = (int *)hashmap_get(cpu->context, "ZF");
    if(zf==NULL) {
        return 1;
    }
    if(*zf == 0){
        int *ip=(int *)hashmap_get(cpu->context, "IP");
        if(ip==NULL) {
            return 1;
        }
        *ip = *(int *) dest;
        return 0;
    }
    return 1;
}

int handle_HALT(CPU* cpu, void* src, void* dest) {
    Segment* cseg = hashmap_get(cpu->memory_handler->allocated, "CS");
    if(cseg==NULL) {
        return 1;
    }
    *(int *)hashmap_get(cpu->context, "IP") = cseg->size;
    return 0;
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

char * trim ( char * str ) {
    while (* str == ' ' || * str == '\t' || * str == '\n' || * str == '\r' ) str ++;

    char * end = str + strlen ( str ) - 1;
    while ( end > str && (* end == ' ' || * end == '\t' || * end == '\n' || * end == '\r')) {
        *end = '\0';
        end--;
    }
    return str ;
}

int search_and_replace ( char ** str , HashMap * values ) {
    if (! str || !* str || ! values ) return 0;
    int replaced = 0;
    char * input = * str ;
    
    // Iterate through all keys in the hashmap
    for ( int i = 0; i < values -> size ; i ++) {
        if ( values -> table [i]. key && values -> table [i]. key != TOMBSTONE) {
            char * key = values -> table [i]. key ;
            int value = * ( int *) values -> table [i]. value ;
            // Find potential substring match
            char * substr = strstr ( input , key ) ;
            if ( substr ) {
                // Construct replacement buffer
                char replacement [64];
                snprintf ( replacement , sizeof ( replacement ) , "%d" , value ) ;
            
                // Calculate lengths
                int key_len = strlen ( key ) ;
                int repl_len = strlen ( replacement ) ;
            
                // Create new string
                char * new_str = ( char *) malloc ( strlen ( input ) - key_len + repl_len +1) ;
                strncpy ( new_str , input , substr - input ) ;
                new_str [ substr - input ] = '\0' ;
                strcat ( new_str , replacement ) ;
                strcat ( new_str , substr + key_len ) ;
            
                // Free and update original string
                free ( input ) ;
                * str = new_str ;
                input = new_str ;
                replaced = 1;
            }
        }
    }
    
    // Trim the final string
    if ( replaced ) {
        char * trimmed = trim ( input ) ;
        if ( trimmed != input ) {
            memmove ( input , trimmed , strlen ( trimmed ) + 1) ;
        }
    }
    return replaced;
}

int resolve_constants(ParserResult *result) {
    for(int i =0; i<result->code_count; i++) {
        if(result->code_instructions[i]->operand2) {
            search_and_replace(&(result->code_instructions[i]->operand2), result->memory_locations);
            //printf("res : %s\n", result->code_instructions[i]->operand2);
        }
    }
    return 0;
}

void allocate_code_segment(CPU *cpu, Instruction **code_instructions, int code_count) {
    Segment* seg_start=hashmap_get(cpu->memory_handler->allocated, "DS");
    int res=0, start=0;
    if(seg_start!=NULL) {
        start=seg_start->start+seg_start->size;
    }
    
    res = create_segment(cpu->memory_handler, "CS", start, code_count);
    
    if(res==1) {
        printf("Cannot create code segment\n");
        return;
    }
    Instruction *ins=NULL;
    for(int i=0; i<code_count; i++) {
        ins=code_instructions[i];
        int* p = malloc(sizeof(int));
        assert(p);
        if(ins->operand2) {
            p = resolve_addressing(cpu, ins->operand2);
        } else if(ins->operand1) {
            p = resolve_addressing(cpu, ins->operand1);
        } else {
            printf("Big issue\n");
            return;
        }
    
        printf("p : %d\n", *p);
        if(p==NULL) {
            printf("Cannot resolve address\n");
            continue;
        }

        if(store(cpu->memory_handler, "CS", i, (void*) p)==NULL) {
            printf("Cannot store code instruction\n");
        }
    }
    
    int *ip = hashmap_get(cpu->context, "IP");
    *ip=0;
}

int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest){
    if(strcmp(instr->mnemonic, "MOV") == 0){
        handle_MOV(cpu, src, dest);
        return 0;
    }
    if(strcmp(instr->mnemonic, "ADD") == 0){
        handle_ADD(cpu, src, dest);
        return 0;
    }
    if(strcmp(instr->mnemonic, "CMP") == 0){
        return handle_CMP(cpu, src, dest);
    }
    if(strcmp(instr->mnemonic, "JMP") == 0){
        return handle_JMP(cpu, src, dest);
    }
    if(strcmp(instr->mnemonic, "JZ") == 0){
        return handle_JZ(cpu, src, dest);
    }
    if(strcmp(instr->mnemonic, "JNZ") == 0){
        return handle_JNZ(cpu, src, dest);
    }
    if(strcmp(instr->mnemonic, "HALT") == 0) {
        return handle_HALT(cpu, src, dest);
    }
    return 1;
}

int execute_instruction(CPU *cpu, Instruction *instr) {
    void * src = resolve_addressing(cpu,instr->operand1);
    if(src==NULL) {return 1;}
    void * dest = resolve_addressing(cpu,instr->operand2);
    if(dest==NULL) {return 1;}
    
    return handle_instruction(cpu, instr, src, dest);
}

Instruction* fetch_next_instruction(CPU *cpu){
    int * ip = (int *)hashmap_get(cpu->context, "IP");
    if(ip==NULL) {
        return NULL;
    }
    Instruction * i = (Instruction *) load(cpu->memory_handler, "CS", *ip);
    if(!i){
        return NULL;
    }
    *ip = *ip+1;
    return i;
}

int run_program(CPU *cpu) {
    // Affichage initial
    print_data_segment(cpu);
    printf("Press enter to execute next instruction or 'q' to quit\n");
    char buffer[5];
    fgets(buffer, 5, stdin);
    Instruction *ins=NULL;
    if(strcmp(buffer, "\n")==0) {
        ins = fetch_next_instruction(cpu);
        execute_instruction(cpu, ins);
    }

    while(ins && fgets(buffer, 5, stdin)) {
        if(strcmp(buffer, "q")==0) {
            return 1;
        }
        
        if(strcmp(buffer, "\n")==0) {
            ins = fetch_next_instruction(cpu);
            execute_instruction(cpu, ins);
        }
    }

    // Affichage final
    print_data_segment(cpu);
    return 0;
}