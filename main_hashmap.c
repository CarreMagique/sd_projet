#include "hashmap.h"
#include <stdio.h>
int main(){
    HashMap* map = hashmap_create();
    int* p = malloc(sizeof(int));
    *p = 7;
    hashmap_insert(map, "bonjour", p);
    printf("%d\n", *(int *)hashmap_get(map,"bonjour"));

    int x = 8;
    int y = 9;
    hashmap_insert(map, "test", &x);
    printf("%d\n", *(int *)hashmap_get(map,"test"));
    hashmap_insert(map, "tset", &y);
    printf("%d\n", *(int *)hashmap_get(map,"tset"));
    hashmap_remove(map, "test");
    void* res = hashmap_get(map,"test");
    if(res!=NULL){
        printf("%d\n", *(int *)hashmap_get(map,"test"));
    }else{
        printf("clé non trouvé\n");
        
    }

    printf("%d\n", *(int *)hashmap_get(map,"tset"));
    x = 32;
    hashmap_insert(map, "test", &x);
    printf("%d\n", *(int *)hashmap_get(map,"test"));
    free(p);
}