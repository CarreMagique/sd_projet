#include "../cpu.h"
#include "../parser.h"
#include <assert.h>
#include <stdio.h>


int main(){
    CPU* cpu = cpu_init(1024);
    ParserResult* pr = parse("tests/test_parser.txt");
    allocate_variables(cpu, pr->data_instructions, pr->data_count);
    resolve_constants(pr);
    allocate_code_segment(cpu, pr->code_instructions, pr->code_count);
    run_program(cpu);
    int ax = * (int *) hashmap_get(cpu->context, "AX");
    assert(ax==52);
    free_parser_result(pr);
    cpu_destroy(cpu);
    return 0;
}