#include "parser.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main() {
    ParserResult* parser = parse("test_assert.txt");
    
    assert(strcmp(parser->data_instructions[0]->mnemonic,"x")==0);
    assert(strcmp(parser->data_instructions[0]->operand1,"DW")==0);
    assert(strcmp(parser->data_instructions[0]->operand2,"42")==0);
    assert(strcmp(parser->code_instructions[0]->mnemonic,"MOV")==0);
    assert(strcmp(parser->code_instructions[0]->operand1,"AX")==0);
    assert(strcmp(parser->code_instructions[0]->operand2,"x")==0);
    
    free_parser_result(parser);
    return 0;
}