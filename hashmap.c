#include "hashmap.h"

unsigned long simple_hash(const char *str){
    long c = 0;
    int i = 0;
    while (str[i]!='\0')
    {
        c = c + str[i];
        i = i+1;
    }
    c= c%128;
    return c;
}
HashMap *hashmap_create(){
    HashMap* map = malloc(sizeof(HashMap));
    map->size = TABLE_SIZE;
    map->table = malloc(sizeof(HashEntry)*map->size);
    for(int i = 0; i < map->size; i++){
        map->table[i].value = NULL;
        map->table[i].key = NULL;
    }
    return map;
}
int hashmap_insert(HashMap *map, const char *key, void *value){
    int k = simple_hash(key);
    if(map->table[k].key==NULL){
        map->table[k].key = strdup(key);
        map->table[k].value = value;
        return 0;
    }
    int i;
    for(i = k+1; i < map->size && map->table[i].key!=NULL; i++){}
    if(i != map->size){
        map->table[i].key = strdup(key);
        map->table[i].value = value;
        return 0;
    }
    return 1;
}

void *hashmap_get(HashMap *map, const char *key){
    int i;
    for(i = simple_hash(key); i < map->size && strcmp(map->table[i].key, key) != 0; i++){}
    if(i != map->size ){
        return map->table[i].value;
    }
    return NULL;
}

int hashmap_remove(HashMap *map, const char *key){
    int i;
    for(i = simple_hash(key); i < map->size && strcmp(map->table[i].key, key) != 0; i++){}
    if(i!=map->size){
        free(map->table[i].key);
        map->table[i].value = TOMBSTONE;
        return 0;
    }
    return 1;
}
void hashmap_destroy(HashMap *map){
    for (int i = 0; i < map->size; i++){
        map->table[i].value = TOMBSTONE;
        free(map->table[i].key)
    }
    free(map->table);
    free(map);
}
