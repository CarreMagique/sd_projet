#include "../cpu.h"
#include "../parser.h"
#include <assert.h>
#include <stdio.h>

CPU * setup_test_environment () {
    // Initialiser le CPU
    CPU * cpu = cpu_init(1024) ;
    if (! cpu ) {
        printf ("Error CPU initalization failed\n") ;
        return NULL ;
    }
    // Initialiser les registres avec des valeurs specifiques
    int * ax = ( int *) hashmap_get ( cpu->context , "AX" ) ;
    int * bx = ( int *) hashmap_get ( cpu->context , "BX" ) ;
    int * cx = ( int *) hashmap_get ( cpu->context , "CX" ) ;
    int * dx = ( int *) hashmap_get ( cpu->context , "DX" ) ;

    * ax = 3;
    * bx = 6;
    * cx = 100;
    * dx = 0;
    Segment * SS = hashmap_get(cpu->memory_handler->allocated, "SS");
    int start=SS->start+SS->size;
    // Creer et initialiser le segment de donnees
    if (hashmap_get(cpu->memory_handler->allocated,"DS")==NULL) {
        create_segment(cpu->memory_handler, "DS", start, 20);

        // Initialiser le segment de donnees avec des valeurs de test
        for ( int i = 0; i < 20; i ++) {
            int * value = ( int *) malloc (sizeof(int));
            * value = i * 10 + 5; // Valeurs 5, 15, 25, 35...
            store (cpu->memory_handler , "DS" , start+i , value );
        }
    }
    printf ("Test environment initialized\n");
    return cpu ;
}

int main(){
    CPU* cpu = cpu_init(1000);
    ParserResult* pr = parse("tests/test_parser.txt");
    allocate_variables(cpu, pr->data_instructions, pr->data_count);
    print_data_segment(cpu);
    
    cpu_destroy(cpu);

    
    CPU* cpu_test = setup_test_environment();
    print_data_segment(cpu_test);
    //On recupere le segment DS pour avoir sa position de depart 
    Segment * DS = hashmap_get(cpu_test->memory_handler->allocated, "DS");
    int start=DS->start;

    int *dest1=load(cpu_test->memory_handler, "DS", start);
    int *src1=resolve_addressing(cpu_test,"42"); //Devrait appeler immediate_addressing
    handle_MOV(cpu_test,src1,dest1);
    assert(* (int*)dest1==42);
    assert((*dest1)==(*src1));

    int *dest2=load(cpu_test->memory_handler, "DS", start+1);
    int *src2=resolve_addressing(cpu_test, "AX"); //Devrait appeler register_addressing
    handle_MOV(cpu_test,src2,dest2);
    assert(* (int*)dest2==3);
    
    int *dest3=load(cpu_test->memory_handler, "DS", start+2);
    int *src3=resolve_addressing(cpu_test, "[0]"); //Devrait appeler memory_direct_addressing
    handle_MOV(cpu_test,src3,dest3);
    assert(* (int *)dest3==42); //Nouvelle valeur de AX
    
    int *dest4=load(cpu_test->memory_handler, "DS", start+3);
    int *src4=resolve_addressing(cpu_test, "[AX]"); //Devrait appeler register_indirect_addressing
    handle_MOV(cpu_test,src4,dest4);
    assert(* (int *)dest4==35); //3*10+5
    print_data_segment(cpu_test);
    free_parser_result(pr);
    cpu_destroy(cpu_test);

    return 0;
}
