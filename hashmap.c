#include "hashmap.h"
//Fonction de hashage, on réalise un hash qui convertie la somme des codes ASCII des caractères en un entier compris entre 0 et TABLE_SIZE
unsigned long simple_hash(const char *str){
    long c = 0;
    int i = 0;
    while (str[i]!='\0')
    {
        c = c + str[i];
        i = i+1;
    }
    c = c%TABLE_SIZE;
    return c;
}
//Création hashmap
HashMap *hashmap_create(){
    HashMap* map = malloc(sizeof(HashMap));
    assert(map);
    map->size = TABLE_SIZE;
    map->table = malloc(sizeof(HashEntry)*map->size);
    assert(map->table);
    //On initialise toutes les cases de la Hashmap à NULL pour éviter les erreurs de segmentation
    for(int i = 0; i < map->size; i++){
        map->table[i].value = NULL;
        map->table[i].key = NULL;
    }
    return map;
}
//Insertion d'une clé/valeur
int hashmap_insert(HashMap *map, const char *key, void *value){
    int k = simple_hash(key);
    int i;
    //Probing lineaire
    for(i = k; i < map->size && map->table[i].key!=NULL && strcmp(map->table[i].key, key) != 0; i++){}
    //On insert si on est pas à la fin de la hashmap
    if(i != map->size){
        if(map->table[i].key==NULL){
            map->table[i].key = strdup(key);
            map->table[i].value = value;
            return 0;
        }
        if(strcmp(map->table[i].key, key) ==0){
            //Si l'on rencontre une TOMBSTONE, on a un emplacement libre
            if(map->table[k].value == TOMBSTONE){
                map->table[k].value = value;
                return 0;
            }
            return 3;
        }
        return 0;
    }
    //Sinon erreur
    return 1;
}

void *hashmap_get(HashMap *map, const char *key){
    int i;
    for(i = simple_hash(key); i < map->size && map->table[i].key!=NULL && strcmp(map->table[i].key, key) != 0; i++){}
    if(i != map->size && map->table[i].value != TOMBSTONE){
        return map->table[i].value;
    }
    return NULL;
}

int hashmap_remove(HashMap *map, const char *key){
    int i;
    for(i = simple_hash(key); i < map->size && strcmp(map->table[i].key, key) != 0; i++){}
    if(i!=map->size){
        map->table[i].value = TOMBSTONE;
        return 0;
    }
    return 1;
}
void hashmap_destroy(HashMap *map){
    for (int i = 0; i < map->size; i++){
        if(map->table[i].value && map->table[i].value!=TOMBSTONE) {
            free(map->table[i].value);
        }
        free(map->table[i].key);
    }
    free(map->table);
    free(map);
}

