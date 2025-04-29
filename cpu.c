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

    int* sp=malloc(sizeof(int));
    *sp=0;
    hashmap_insert(cpu->context, "SP", sp);

    int* bp=malloc(sizeof(int));
    *bp=0;
    hashmap_insert(cpu->context, "BP", bp);

    int* es=malloc(sizeof(int));
    *es=-1;
    hashmap_insert(cpu->context, "ES", es);

    int res =create_segment(cpu->memory_handler, "SS", 0, 128);
    if(res!=0) {
        printf("Cannot create Stack Segment\n");
    }
    
    return cpu;
}

void cpu_destroy(CPU *cpu) {
    free_memory_handler(cpu->memory_handler);
    hashmap_destroy(cpu->context);
    hashmap_destroy(cpu->constant_pool);
    free(cpu);
}
//Retourne la donnee dans le segment segment_name et a la position pos dans la memoire du MemoryHandler
void* load(MemoryHandler *handler, const char *segment_name, int pos){
    Segment* smg = hashmap_get(handler->allocated, segment_name);
    if(!smg){
        return NULL;
    }
    if(smg->start+smg->size<pos || pos>handler->total_size){
        return NULL;
    }
    void* data = handler->memory[pos];
    return data;
}
//Stocke la donnee data dans le segment segment_name et a la position pos dans la memoire du MemoryHandler
void* store(MemoryHandler *handler, const char *segment_name,int pos, void *data) {
    Segment *seg = hashmap_get(handler->allocated, segment_name);
    if(seg==NULL) { return NULL;}
    if(pos>seg->size+seg->start || pos>handler->total_size) { return NULL;}
    handler->memory[pos]=data;
    return data;
}
//Alloue les variables de la partie .DATA et stocke les valeurs dans le segment DS
void allocate_variables(CPU *cpu, Instruction** data_instructions, int data_count) {
    Instruction *ins;
    int size = 0;
    //On determine la taille necessaire pour creer le segment
    for(int i=0; i<data_count; i++) {
        ins=data_instructions[i];
        for(int j = 0; ins->operand2[j]!='\0'; j++){
            if(ins->operand2[j] == ','){
                size = size+1;
            }
        }
        size = size+1;
    }
    Segment* seg= (Segment *) hashmap_get(cpu->memory_handler->allocated, "SS");
    int start = 0;
    if(seg) {
        start=seg->start+seg->size;
    }
    int res = create_segment(cpu->memory_handler, "DS", start, size);
    if(res == 1){
        printf("Cannot create segment DS\n");
        return;
    }
    int pos = start; //position start+size
    int *p=NULL;
    for(int i=0; i<data_count; i++) {
        ins=data_instructions[i];
        char buffer[100];
        for(int i=0; i<100; i++) {buffer[i]='\0';} //On initialise le buffer
        int b = 0;
        for(int j = 0; ins->operand2[j]!='\0'; j++){
            buffer[b] = ins->operand2[j];
            b++;
            if(ins->operand2[j] == ','){
                p = malloc(sizeof(int));
                *p= atoi(buffer);
                store(cpu->memory_handler, "DS", pos, (void*) p);
                pos++;
                b = 0;
            }
        }
        //On traite le dernier cas qui n'est pas traite dans la boucle
        p = malloc(sizeof(int));
        *p= atoi(buffer);
        store(cpu->memory_handler, "DS", pos, (void*) p);
        pos++;
    }
}
//Affiche les valeurs stcokees dans le segment DS
void print_data_segment(CPU *cpu) {
    Segment *DS = hashmap_get(cpu->memory_handler->allocated, "DS");
    if(DS==NULL) {
        printf("DS is NULL\n");
        return;
    }
    for(int i=DS->start; i <DS->start+DS->size; i++) {
        if(cpu->memory_handler->memory[i]) {
            printf("%d\t",* (int *)(cpu->memory_handler->memory[i]));
        } else {
            printf("X\t"); //N'arrive que si la case n'est pas initialisee -> bug
        }
        
    }
    printf("\n");
}
//Retourne si l'expression matche le pattern regex
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
/*--------------------ADRESSING--------------------*/
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
        Segment * seg = (Segment *) (hashmap_get(cpu->memory_handler->allocated, "DS"));
        int start =  seg->start;
        int data=0;
        sscanf(operand, "[%d]", &data);
        return cpu->memory_handler->memory[start+data];
    }
    return NULL;
}

void *register_indirect_addressing(CPU *cpu, const char*operand) {
    if(matches("^\\[[A-D]X\\]$",operand)) {
        char buffer[3];
        sscanf(operand, "[%s]", buffer);
        buffer[2]='\0'; //sinon seg fault
        Segment * seg = (Segment *) (hashmap_get(cpu->memory_handler->allocated, "DS"));
        int start =  seg->start;
        int* index = hashmap_get(cpu->context, buffer);
        if(index!=NULL) {
            return cpu->memory_handler->memory[*index+start];
        }        
        return NULL;
    }
    return NULL;
}

void* segment_override_addressing(CPU* cpu, const char* operand){
    if(matches("^\\[[C-E]S:[A-D]X\\]$",operand)) {
       char segment[3];
       char registre[3];
       //sscanf(operand, "[%s:%s]", segment, registre);
       segment[0]=operand[1];
       segment[1]=operand[2];
       segment[2]='\0';
       registre[0]=operand[4];
       registre[1]=operand[5];
       registre[2]='\0';
       return load(cpu->memory_handler, segment, *(int *)register_addressing(cpu, registre));
    }
    return NULL;
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
    res = segment_override_addressing(cpu, operand);
    if(res){
        return res;
    }
    return NULL;
}

int push_value(CPU *cpu, int value) {
    int *sp=hashmap_get(cpu->context, "SP");
    (*sp)--;
    if(*sp<0) {
        return -1;
    }
    if(*sp>cpu->memory_handler->total_size) {
        return -1;
    }
    int *val=malloc(sizeof(int));
    *val=value;
    cpu->memory_handler->memory[*sp]=val;
    return 0;
}

int pop_value(CPU *cpu, int *dest) {
    int *sp=hashmap_get(cpu->context, "SP");
    (*sp)++;
    if(*sp<0) {
        return -1;
    }
    if(*sp>cpu->memory_handler->total_size) {
        return -1;
    }
    *dest=*(int *)cpu->memory_handler->memory[*sp];
    return 0;
}

/*--------------------HANDLE--------------------*/

int handle_MOV(CPU* cpu, void* src, void* dest) {
    if(src==NULL || dest==NULL) {return 1;}
    *(int *)dest=*(int *)src;
    return 0;
}

int handle_ADD(CPU* cpu, void* src, void* dest){
    if(src==NULL || dest==NULL) {return 1;}
    *(int *)dest=*(int *)src+*(int *)dest;
    return 0;
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
    if(ip==NULL || dest==NULL) {
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

int handle_PUSH(CPU* cpu, void* src) {
    if(src==NULL) {
        int *a = hashmap_get(cpu->context, "AX");
        return push_value(cpu, *a);
    }    
    return push_value(cpu, * (int *) src);
}

int handle_POP(CPU* cpu, void* dest) {
    if(dest==NULL) {
        int *a = hashmap_get(cpu->context, "AX");
        return pop_value(cpu, a);
    }
    return pop_value(cpu, dest);
}

int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest){
    if(strcmp(instr->mnemonic, "MOV") == 0){
        return handle_MOV(cpu, src, dest);
    }
    if(strcmp(instr->mnemonic, "ADD") == 0){
        return handle_ADD(cpu, src, dest);
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
    if(strcmp(instr->mnemonic, "PUSH") == 0){
        return handle_PUSH(cpu, src);
    }
    if(strcmp(instr->mnemonic, "POP") == 0) {
        return handle_POP(cpu, dest);
    }
    if(strcmp(instr->mnemonic, "ALLOC") == 0) {
        return alloc_es_segment(cpu);
    }
    if(strcmp(instr->mnemonic, "FREE") == 0) {
        return free_es_segment(cpu);
    }
    return 1;
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
            char * key = values -> table [i]. key;
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
//Remplace le nom d'une variable par son adresse en memoire dans l'instruction
int resolve_constants(ParserResult *result) {
    for(int i =0; i<result->code_count; i++) {
        if(result->code_instructions[i]->operand2) {
            search_and_replace(&(result->code_instructions[i]->operand2), result->memory_locations);
            // On transforme la valeur "i" en "[i]"
            char temp = * result->code_instructions[i]->operand2;
            free(result->code_instructions[i]->operand2);
            result->code_instructions[i]->operand2=(char *) malloc(sizeof(char)*4);
            result->code_instructions[i]->operand2[0]='[';
            result->code_instructions[i]->operand2[1]=temp;
            result->code_instructions[i]->operand2[2]=']';
            result->code_instructions[i]->operand2[3]='\0';
        } else if (result->code_instructions[i]->operand1) {
            search_and_replace(&(result->code_instructions[i]->operand1), result->labels);
            // On transforme la valeur "i" en "[i]"
            char temp = * result->code_instructions[i]->operand1;
            free(result->code_instructions[i]->operand1);
            result->code_instructions[i]->operand1=(char *) malloc(sizeof(char)*4);
            result->code_instructions[i]->operand1[0]='[';
            result->code_instructions[i]->operand1[1]=temp;
            result->code_instructions[i]->operand1[2]=']';
            result->code_instructions[i]->operand1[3]='\0';
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
        
        if(ins->operand2) {
            resolve_addressing(cpu, ins->operand2);
        } else if(ins->operand1) {
            resolve_addressing(cpu, ins->operand1);
        } else if(ins->mnemonic){
            resolve_addressing(cpu, ins->mnemonic);
        }

        if(store(cpu->memory_handler, "CS", i, (void*) ins)==NULL) { //Instruction a dupliquer plus tard
            printf("Cannot store code instruction\n");
        }
    }
    
    int *ip = hashmap_get(cpu->context, "IP");
    *ip=0;
}

int execute_instruction(CPU *cpu, Instruction *instr) {
    if(instr==NULL) {
        return 1;
    }
    void * dest = NULL;
    void * src = NULL;
    if(instr->operand1==NULL) {
        dest=resolve_addressing(cpu, instr->mnemonic);
        return handle_instruction(cpu, instr, src, dest);
    }
    dest=resolve_addressing(cpu,instr->operand1);    
    if(instr->operand2) {
        src = resolve_addressing(cpu,instr->operand2);
    }
    return handle_instruction(cpu, instr, src, dest);
}

Instruction* fetch_next_instruction(CPU *cpu){
    int * ip = (int *)hashmap_get(cpu->context, "IP");
    if(ip==NULL) {
        return NULL;
    }
    if(*ip>=cpu->memory_handler->total_size || *ip<0) {
        return NULL;
    }
    Instruction* ins = (Instruction *) load(cpu->memory_handler, "CS", *ip);
    
    (*ip)++;
    return ins;
}

int alloc_es_segment(CPU *cpu) {
    int seg_size = * (int *) hashmap_get(cpu->context, "AX");
    int strategy = * (int *) hashmap_get(cpu->context, "BX");
    int* zf = (int *) hashmap_get(cpu->context, "ZF");
    int res = find_free_address_strategy(cpu->memory_handler, seg_size, strategy);
    if(res==-1) {
        *zf=1;
        return 1;
    } else {
        *zf=0;
        if (create_segment(cpu->memory_handler, "ES", res, seg_size)!=0) {
            printf("Cannot create ES\n");
            return 3;
        }
        Segment *seg = (Segment *) hashmap_get(cpu->memory_handler->allocated, "ES");
        for(int i=seg->start; i<seg->start+seg->size; i++) {
            int *val = malloc(sizeof(int));
            *val=0;
            (cpu->memory_handler->memory[i])=val;
        }
        *(int *) hashmap_get(cpu->context, "ES") = seg->start;
        return 0;
    }
    return 2;
}
int free_es_segment(CPU* cpu){
    Segment * seg = hashmap_get(cpu->memory_handler->allocated, "ES");
    for(int i = seg->start; i<seg->start+seg->size; i++){
        void* data = cpu->memory_handler->memory[i];
        free(data);
        cpu->memory_handler->memory[i] = NULL;
    }
    remove_segment(cpu->memory_handler, "ES");
    return 0;
}

int run_program(CPU *cpu) {
    // Affichage initial
    print_data_segment(cpu);
    printf("Press enter to execute next instruction or 'q' to quit\n");
    char buffer[3];
    fgets(buffer, 3, stdin);
    Instruction *ins=NULL;
    if(strcmp(buffer, "\n")==0) {
        ins = fetch_next_instruction(cpu);
        if(ins==NULL) {return 0;}
        if(execute_instruction(cpu, ins)!=0) {
            printf("Cannot execute instruction\n");
        }
    }

    while(ins && fgets(buffer, 3, stdin)) {
        if(strcmp(buffer, "q")==0) {
            return 1;
        }
        
        if(strcmp(buffer, "\n")==0) {
            ins = fetch_next_instruction(cpu);
            if(ins==NULL) {break;}
            if(execute_instruction(cpu, ins)!=0) {
                printf("Cannot execute instruction\n");
            }
            printf("%s\n", ins->mnemonic);
        }
    }

    // Affichage final
    print_data_segment(cpu);
    return 0;
}