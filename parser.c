#include "parser.h"

Instruction *parse_data_instruction(const char *line, HashMap *memory_locations, int data_count) {
    char* word=(char *)malloc(sizeof(char)*10);
    int i=0, j=0, count=0;
    Instruction *ins=(Instruction *)malloc(sizeof(Instruction));
    /*Pas de stress, espace plaisir*/
    ins->mnemonic=NULL;
    ins->operand1=NULL;
    ins->operand2=NULL;

    while(line[i]) {
        if(line[i]==' ') {
            switch(count) {
                case 0:
                    ins->mnemonic=strdup(word);
                    hashmap_insert(memory_locations,ins->mnemonic,(void*)&data_count);
                case 1:
                    ins->operand1=strdup(word);
                case 2:
                    ins->operand2=strdup(word);
                    break;
            }
            count++;
            free(word);
            word=(char *)malloc(sizeof(char)*10);
            j=0;
        }
        word[j]=line[i];
        j++;
        i++;
    }
    return ins;
}

Instruction *parse_code_instruction(const char *line, HashMap *labels, int code_count){
    char* word=(char *)malloc(sizeof(char)*10);
    Instruction *ins=(Instruction *)malloc(sizeof(Instruction));
    /*Pas de stress, espace plaisir*/
    ins->mnemonic=NULL;
    ins->operand1=NULL;
    ins->operand2=NULL;
    int i=0, j=0, count=0;
    while(line[i]) {
        if(line[i]==' ' || line[i] == ',') {
            switch(count) {
                case 0:
                    if(word[j-1] == ':'){
                        word[j-1] = '\0';
                        count = -1;
                        hashmap_insert(labels,strdup(word),(void*)&code_count);
                    }else{
                        ins->mnemonic=strdup(word);
                    }
                case 1:
                    ins->operand1=strdup(word);
                case 2:
                    ins->operand2=strdup(word);
                    break;
            }
            count++;
            free(word);
            word=(char *)malloc(sizeof(char)*10);
            j=0;
        }
        word[j]=line[i];
        j++;
        i++;
    }
    if(ins->operand2==NULL){
        ins->operand2 = strdup("");
    }
    return ins;

}

Instruction** resize_tab(Instruction** tab, int size){
    Instruction** tmp = malloc(sizeof(Instruction*)*(size+1));
    for(int i = 0; i<size; i++){
        *tmp[i] = *tab[i];
    }
    free(tab);
    return tmp;
}

ParserResult *parse(const char *filename){
    FILE* file = fopen(filename, "r");
    char buffer[100];
    int endroit = -1;
    ParserResult* pr = malloc(sizeof(ParserResult));
    pr->data_count = 0;
    pr->code_count = 0;
    pr->data_instructions = NULL;
    pr->code_instructions = NULL;
    pr->memory_locations = hashmap_create();
    pr->labels = hashmap_create();

    while(fgets(buffer, 100, file)){
        if(strcmp(buffer, ".DATA") == 0){
            endroit = 0;
        }
        else if(strcmp(buffer, ".CODE") == 0){
            endroit = 1;
        }else{
            switch(endroit){
                case 0:
                    pr->data_count++;
                    pr->data_instructions = resize_tab(pr->data_instructions,pr->data_count);
                    pr->data_instructions[pr->data_count-1] = parse_data_instruction(buffer,pr->memory_locations,pr->data_count);
                case 1:
                    pr->code_count++;
                    pr->code_instructions = resize_tab(pr->code_instructions,pr->code_count);
                    pr->code_instructions[pr->code_count-1] = parse_code_instruction(buffer,pr->labels, pr->code_count);
            }
        }
    }
    fclose(file);
    return pr;
}

void free_parser_result(ParserResult *result) {
    hashmap_destroy(result->memory_locations);
    hashmap_destroy(result->labels);
    free(result->code_instructions);
    free(result->data_instructions);
    free(result);
}