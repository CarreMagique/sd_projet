#include "memoryHandler.h"
#include "hashmap.h"

MemoryHandler *memory_init(int size) {
    MemoryHandler* mem = (MemoryHandler *)malloc(sizeof(MemoryHandler));
    mem->total_size=size;
    mem->memory=malloc(sizeof(void *)*mem->total_size);
    
    mem->free_list=(Segment *)malloc(sizeof(Segment));
    mem->free_list->size=mem->total_size;
    mem->free_list->start=0;
    mem->free_list->next=NULL;
    
    mem->allocated=hashmap_create();
    return mem;
}

Segment *find_free_segment(MemoryHandler* handler, int start, int size, Segment** prev) {
    Segment *seg=handler->free_list;
    if(seg==NULL) {
        return NULL;
    }
    if(seg->start<=start && seg->start+seg->size >= start+size) {
        *prev=NULL;
        return seg;
    }
    while(seg->next) {
        if(seg->next->start<=start && seg->next->start+seg->next->size >= start+size) {
            *prev=seg;
            return seg->next;
        }
        seg=seg->next;
    }

    *prev=NULL;
    return NULL;
}

int create_segment(MemoryHandler *handler, const char *name, int start, int size) {
    if(start+size>handler->total_size) { //Dans ce cas le segment est trop large pour etre cree
        return 1;
    }
    Segment *prev=NULL;
    Segment* seg=find_free_segment(handler,start,size,&prev);
    if(seg==NULL) {
        return 1;
    }
    Segment* new_seg=(Segment *)malloc(sizeof(Segment));

    new_seg->size=size;
    new_seg->start=start;
    new_seg->next=NULL;
    if(hashmap_insert(handler->allocated, name, new_seg)==3) {
        return 1;
    }
    Segment *seg1=NULL;
    Segment *seg2=NULL;

/*On cree un segment si le segment seg ne debute pas a la meme adresse que start*/
    if(seg->start!=start){
        seg1=(Segment *)malloc(sizeof(Segment));
        seg1->start=seg->start; //Meme debut que seg
        seg1->size=start-seg->start; //La taille est la "longueur" entre le debut de seg et start
        if(prev!=NULL){
            prev->next = seg1;
            prev=prev->next;
        }else{
            handler->free_list = seg1;
            prev=seg1;
        }
    }

/*On cree un segment si le segment seg ne termine pas a la meme adresse que start+size*/
    if(start+size!=seg->start+seg->size){
        seg2=(Segment *)malloc(sizeof(Segment));
        seg2->start=start+size; //seg2 commence a la fin du segment alloue
        seg2->size=seg->start + seg->size - seg2->start; //La taille de seg2 est la "longueur" entre la fin du segment alloue et la fin de seg
        if(prev) {
            prev->next=seg2;
            prev=prev->next;
        } else {
            handler->free_list=seg2;
            prev=seg2;
        }
    }

/*On essaie d'ajouter la suite de la liste dans l'ordre des segments : seg2, seg1, debut de free_list*/
    if(seg2) {
        seg2->next=seg->next;
    } else if(seg1) {
        seg1->next=seg->next;
    } else {
        handler->free_list=seg->next;
    }
    free(seg);

    return 0;
}

int remove_segment(MemoryHandler *handler, const char *name) {
    Segment* seg =(Segment *)hashmap_get(handler->allocated,name);
    hashmap_remove(handler->allocated,name);
    return 0;
}
