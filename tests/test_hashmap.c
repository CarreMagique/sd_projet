#include "../hashmap.h"
int main(){
    HashMap* map = hashmap_create();
    assert(map);
    int* p = malloc(sizeof(int));
    *p = 7;
    assert(hashmap_insert(map, "bonjour", p) ==0);
    assert(*(int *)hashmap_get(map,"bonjour") == 7);

    int x = 8;
    int y = 9;
    assert(hashmap_insert(map, "test", &x) ==0);
    assert(*(int *)hashmap_get(map,"test") == 8);
    assert(hashmap_insert(map, "tset", &y) == 0);
    assert(*(int *)hashmap_get(map,"tset") ==9);
    assert(hashmap_remove(map, "test") ==0);
    void* res = hashmap_get(map,"test");
    assert(res == NULL);

    assert(*(int *)hashmap_get(map,"tset") == 9);
    x = 32;
    assert(hashmap_insert(map, "test", &x)==0);
    assert(*(int *)hashmap_get(map,"test") == 32);
    free(p);
    hashmap_destroy(map);
    return 0;
}