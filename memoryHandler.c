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
    if(seg->start<=start && seg->size>=start+size) {
        *prev=NULL;
        return seg;
    }
    while(seg->next) {
        if(seg->next->start<=start && seg->next->size>=start+size) {
            *prev=seg;
            return seg->next;
        }
        seg=seg->next;
    }
    *prev=NULL;
    return NULL;
}

int create_segment(MemoryHandler *handler, const char *name, int start, int size) {
    Segment* seg=find_free_segment(handler,start,size,&seg);
    if(seg) {
        Segment* new_seg=(Segment *)malloc(sizeof(Segment));
        new_seg->size=size;
        new_seg->start=start;
    }
    //Ajouter à la table de hachage allocated avec la cle X

    return 0;
}

int remove_segment(MemoryHandler *handler, const char *name) {
    return 0;
}
