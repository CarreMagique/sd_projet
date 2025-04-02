#include "parser.h"
#define WORD_SIZE 32

int var_count;
Instruction *parse_data_instruction(const char *line, HashMap *memory_locations, int data_count) {
    char* word=(char *)malloc(sizeof(char)*WORD_SIZE);
    assert(word);
    int i=0, j=0, count=0;
    Instruction *ins=(Instruction *)malloc(sizeof(Instruction));
    assert(ins);
    
    ins->mnemonic=NULL;
    ins->operand1=NULL;
    ins->operand2=NULL;

    while(line[i]) {
        if(line[i]!=' ' && line[i]!='\n') { //Si line verifie cela, alors le caractere fait partie d'un mot
            word[j]=line[i];
            j++;
        }else if(line[i]=='\n') { //On arrive a la fin de la ligne
            word[j]='\0';
            if(count==1) {
                ins->operand1=strdup(word);
            } else if(count>=2) {
                ins->operand2=strdup(word);
            }
            free(word);
            word=NULL;
            break;
        } else {
            if(count==0) { //Premier mot : mnemonic
                word[j]='\0';
                ins->mnemonic=strdup(word);
                int* d =malloc(sizeof(int));
                *d = data_count+var_count;
                hashmap_insert(memory_locations,ins->mnemonic,d);
                free(word);
                word=(char *)malloc(sizeof(char)*WORD_SIZE);
                j=0;
            } else if(count==1) { //Deuxieme mot : operand1
                word[j]='\0';
                ins->operand1=strdup(word);
                free(word);
                word=(char *)malloc(sizeof(char)*WORD_SIZE);
                j=0;
            } else {
                word[j]=line[i];
                j++;
            }
            count++;
        }
        if(line[i] == ','){
            var_count += 1;
        }
        i++;
    }
    if(word) {free(word);} //Si l'on est pas rentre dans la boucle, il faut liberer word
    return ins;
}

Instruction *parse_code_instruction(const char *line, HashMap *labels, int code_count){
    char* word=(char *)malloc(sizeof(char)*WORD_SIZE);
    int i=0, j=0, count=0;
    Instruction *ins=(Instruction *)malloc(sizeof(Instruction));
    assert(ins);

    ins->mnemonic=NULL;
    ins->operand1=NULL;
    ins->operand2=NULL;

    while(line[i]) {
        if(line[i]!=' ' && line[i] != ',' && line[i]!='\n') { //Dans ce cas les virgules sont aussi a ignorer
            word[j]=line[i];
            j++;
        }else if(line[i]=='\n') {
            word[j]='\0';
            if(count==1) {
                ins->operand1=strdup(word);
            } else if(count>=2) {
                ins->operand2=strdup(word);
            }
            free(word);
            word=NULL;
            break;
        } else {
            if(count==0) {
                if(word[j-1] == ':'){ //Il s'agit alors du label de l'instruction
                    word[j-1] = '\0';
                    int* d = malloc(sizeof(int));
                    *d = code_count;
                    hashmap_insert(labels,word,d);
                    count--; //Ce mot n'est pas une instruction, on reinitialise donc le compteur
                }else{
                    word[j]='\0';
                    ins->mnemonic=strdup(word);
                }
                free(word);
                word=(char *)malloc(sizeof(char)*WORD_SIZE);
                j=0;
            } else if(count==1) {
                word[j]='\0';
                ins->operand1=strdup(word);
                free(word);
                word=(char *)malloc(sizeof(char)*WORD_SIZE);
                j=0;
            }
            count++;
        }
        i++;
    }

    if(line[i]=='\0') {
        word[j]='\0';
        if(count==1) {
            ins->operand1=strdup(word);
        } else {   
            ins->operand2=strdup(word);
        }
    }
    if(word) {free(word);}
    return ins;
}

Instruction** resize_tab(Instruction** tab, int* size){
    (*size)++;
    Instruction** tmp = malloc(sizeof(Instruction*)*(*size));
    assert(tmp);
    for(int i = 0; i<*size-1; i++){
        tmp[i] = tab[i];
    }
    free(tab);
    return tmp;
}

ParserResult *parse(const char *filename){
    FILE* file = fopen(filename, "r");
    assert(file);
    char buffer[100];
    int endroit = -1;
    ParserResult* pr = malloc(sizeof(ParserResult));
    assert(pr);
    pr->data_count = 0;
    pr->code_count = 0;
    pr->data_instructions = NULL;
    pr->code_instructions = NULL;
    pr->memory_locations = hashmap_create();
    pr->labels = hashmap_create();
    var_count = 0;
    while(fgets(buffer, 100, file)){
        if(strcmp(buffer, ".DATA\n") == 0){
            endroit = 0;
        }
        else if(strcmp(buffer, ".CODE\n") == 0){
            endroit = 1;
        }else{
            if(endroit==0) {
                pr->data_instructions = resize_tab(pr->data_instructions,&pr->data_count);
                pr->data_instructions[pr->data_count-1] = parse_data_instruction(buffer,pr->memory_locations, pr->data_count-1);
            } else if(endroit==1) {
                pr->code_instructions = resize_tab(pr->code_instructions,&pr->code_count);
                pr->code_instructions[pr->code_count-1] = parse_code_instruction(buffer,pr->labels, pr->code_count-1);
            }
        }
    }
    fclose(file);
    return pr;
}

void free_instruction(Instruction *ins) {
    free(ins->mnemonic);
    free(ins->operand1);
    free(ins->operand2);
    free(ins);
}

void free_parser_result(ParserResult *result) {
    for(int i = 0; i<result->data_count; i++){
        free_instruction(result->data_instructions[i]);
    } 
    
    for(int i = 0; i<result->code_count; i++){
        free_instruction(result->code_instructions[i]);
    }
    
    hashmap_destroy(result->memory_locations);
    hashmap_destroy(result->labels);
    free(result->code_instructions);
    free(result->data_instructions);
    free(result);
}
