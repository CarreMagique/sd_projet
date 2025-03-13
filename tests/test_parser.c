#include "../parser.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main() {
    ParserResult* parser = parse("tests/test_parser.txt");
    
    assert(strcmp(parser->data_instructions[0]->mnemonic,"x")==0);
    assert(strcmp(parser->data_instructions[0]->operand1,"DW")==0);
    assert(strcmp(parser->data_instructions[0]->operand2,"42")==0);

    assert(strcmp(parser->data_instructions[1]->mnemonic,"arr")==0);
    assert(strcmp(parser->data_instructions[1]->operand1,"DB")==0);
    assert(strcmp(parser->data_instructions[1]->operand2,"20 ,21 ,22 ,23")==0);
  
    assert(parser->data_count==3);
    assert( * (int *)hashmap_get(parser->memory_locations,"x")==0);
    assert( * (int *)hashmap_get(parser->memory_locations,"y")==5);

    assert(strcmp(parser->code_instructions[0]->mnemonic,"MOV")==0);
    assert(strcmp(parser->code_instructions[0]->operand1,"AX")==0);
    assert(strcmp(parser->code_instructions[0]->operand2,"x")==0);

    assert(strcmp(parser->code_instructions[1]->mnemonic,"ADD")==0);
    assert(strcmp(parser->code_instructions[1]->operand1,"AX")==0);
    assert(strcmp(parser->code_instructions[1]->operand2,"y")==0);

    assert(strcmp(parser->code_instructions[2]->mnemonic,"JMP")==0);
    assert(strcmp(parser->code_instructions[2]->operand1,"loop")==0);
    assert(parser->code_instructions[2]->operand2==NULL);

    assert(parser->code_count==3);
    assert( * (int *)hashmap_get(parser->labels,"start")==0);
    assert( * (int *)hashmap_get(parser->labels,"loop")==1);
    
    free_parser_result(parser);
    return 0;
}