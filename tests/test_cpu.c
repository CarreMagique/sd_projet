#include "../cpu.h"
int main(){
    CPU* cpu = cpu_init(1000);
    int i = 7;
    store(cpu->memory_handler, "espace_plaisir", 1, &i);
}