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
                    hashmap_insert(memory_locations,ins->mnemonic,data_count);
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
                        word[j-1] = '\0'
                        count = -1
                        hashmap_insert(labels,strdup(word),data_count);
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

ParserResult *parse(const char *filename){
    FILE* file = fopen(filename, "r");
    char buffer[100];
    int endroit = -1;
    int code_count = 0;
    int data_count = 0;
    ParserResult* pr = malloc(sizeof(ParserResult));
    pr->data_count = 0;
    pr->code_count = 0;
    int size = 1;
    pr->data_instructions = malloc(sizeof(Instruction*)*size);
    pr->code_instructions = malloc(sizeof(Instruction*)*size);
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
                parse_data_instruction(buffer,pr->memory_locations,pr->data_count);
                pr->data_count = pr->data_count+1;
            case 1:
                parse_code_instruction(buffer,pr->labels, pr->code_count);
                pr->code_count = pr->code_count+1;
            }
        }
    }
}   