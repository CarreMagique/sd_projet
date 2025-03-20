#include "../cpu.h"
#include "../parser.h"
int main(){
    CPU* cpu = cpu_init(1000);
    ParserResult* pr = parse("tests/test_parser.txt");
    allocate_variables(cpu, pr->data_instructions, pr->data_count);
    print_data_segment(cpu);
    cpu_destroy(cpu);
    free_parser_result(pr);
}
