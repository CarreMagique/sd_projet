#include "../cpu.h"
int main(){
    CPU* cpu = cpu_init(1000);
    printf("hello\n");
    int i = 7;
    store(cpu->memory_handler, "espace_plaisir", 1, &i);
    printf("salut\n");
    printf("%d\n",*(int *)load(cpu->memory_handler, "espace_plaisir", 1));
}