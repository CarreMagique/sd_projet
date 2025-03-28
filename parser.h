#ifndef PARSERH
#define PARSERH
#include "hashmap.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct {
    char * mnemonic ; // Instruction mnemonic (ou nom de variable pour .DATA)
    char * operand1 ; // Premier operande (ou type pour .DATA)
    char * operand2 ; // Second operande (ou initialisation pour .DATA)
} Instruction;

typedef struct {
    Instruction ** data_instructions ; // Tableau d’instructions .DATA
    int data_count ; // Nombre d’instructions .DATA
    Instruction ** code_instructions ; // Tableau d’instructions .CODE
    int code_count ; // Nombre d’instructions .CODE
    HashMap * labels ; // labels -> indices dans code_instructions
    HashMap * memory_locations ; // noms de variables -> adresse memoire
} ParserResult;

Instruction *parse_data_instruction(const char *line, HashMap *memory_locations, int data_count);
Instruction *parse_code_instruction(const char *line, HashMap *labels, int code_count);
ParserResult *parse(const char *filename);
void free_instruction(Instruction *ins);
void free_parser_result(ParserResult *result);
#endif