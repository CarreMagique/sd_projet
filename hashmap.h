#ifndef HASHMAP
#define HASHMAP
#define TOMBSTONE (( void *) -1)
typedef struct hashentry {
    char * key ;
    void * value ;
} HashEntry ;
typedef struct hashmap {
    int size ;
    HashEntry * table ;
} HashMap ;
#endif