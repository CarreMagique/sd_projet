#include "memoryHandler.h"

MemoryHandler *memory_init(int size) {
    MemoryHandler* mem = (MemoryHandler *)malloc(sizeof(MemoryHandler));
    assert(mem);
    mem->total_size=size;
    mem->memory=malloc(sizeof(void *)*mem->total_size);
    assert(mem->memory);
    for(int i=0; i<mem->total_size; i++) {
        mem->memory[i]=NULL; //On initialise les valeurs de la mémoire
    }
    mem->free_list=(Segment *)malloc(sizeof(Segment));
    assert(mem->free_list);
    mem->free_list->size=mem->total_size;
    mem->free_list->start=0;
    mem->free_list->next=NULL;
    
    mem->allocated=hashmap_create();
    return mem;
}
//Retourne (s'il existe) un segment libre contenant un segment commencant a start et de taille size
Segment *find_free_segment(MemoryHandler* handler, int start, int size, Segment** prev) {
    Segment *seg=handler->free_list;
    if(seg==NULL) {
        return NULL;
    }
    *prev=NULL; //Valeur par defaut si on ne trouve pas de segment
    if(seg->start<=start && seg->start+seg->size >= start+size) { //Le premier satisfait les conditions
        return seg;
    }
    while(seg->next) {
        if(seg->next->start<=start && seg->next->start+seg->next->size >= start+size) {
            *prev=seg;
            return seg->next;
        }
        seg=seg->next;
    }

    return NULL;
}
/*
Essaye de creer un segment commencant a start et de taille size
Met a jour la free_list et la hashmap allocated
*/
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
    assert(new_seg);

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
        assert(seg1);
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
        assert(seg2);
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
//Supprime le segment name alloue et met a jour la free_list
int remove_segment(MemoryHandler *handler, const char *name) {
    Segment *seg = hashmap_get(handler->allocated,name);
    Segment *prec = handler->free_list;
    Segment *temp=NULL;
    hashmap_remove(handler->allocated,name); //On le supprime des maintenant pour eviter d'avoir a traiter chaque cas
    if(prec==NULL) {
        handler->free_list=seg;
        return 0;
    }
    /*On doit parcourir avec next pour la troisieme situation ou le prochain segment libre n'est pas adjacent*/
    while(prec->next) {
        if(prec->next->start+prec->next->size==seg->start) {
            prec->next->size+=seg->size;
            /*Si le segment etait entre deux segments libres, on peut juste liberer seg et faire 1 grand segment libre*/
            if(prec->next->next && prec->next->next->start==prec->next->start+prec->next->size) {
                prec->next->size+=prec->next->next->size;
                free(prec->next->next);
                prec->next->next=prec->next->next->next;
            }
            free(seg);
            return 0;
        }
        if(prec->next->start==seg->start+seg->size) {
            prec->next->start=seg->start;
            prec->next->size+=seg->size;
            free(seg);
            return 0;
        }
        if(prec->next->start > seg->start) {
            temp=prec->next;
            prec->next=seg;
            seg->next=temp;
        }
        prec->next=prec->next->next;
    }
    if(temp==NULL) {
        free(seg);
    }
    return 0;
}
//Trouve le premier segment libre plus grand ou egal a size
Segment* find_base_fit(MemoryHandler* handler, int size){
    Segment *seg=handler->free_list;
    if(seg==NULL) {
        return NULL;
    }
    if(seg->size >= size) {
        return seg;
    }
    while(seg) {
        if(seg->size >=size) {
            return seg;
        }
        seg=seg->next;
    }
    return NULL;
}

//Trouve le segment libre ayant la taille la plus proche de size et superieur ou egal a size
Segment* find_best_fit(MemoryHandler* handler, int size){
    Segment *seg=handler->free_list;
    if(seg==NULL) {
        return NULL;
    }
    if(seg->size >= size) {
        return seg;
    }
    Segment* min = NULL;
    while(seg) {
        if(seg->size >=size && (min == NULL ||seg->size<min->size)) {
            min = seg;
        }
        seg=seg->next;
    }
    return min;
}
//Trouve le segment libre ayant la plus grande taille et une taille supeieure ou egale a size
Segment* find_worst_fit(MemoryHandler* handler, int size){
    Segment *seg=handler->free_list;
    if(seg==NULL) {
        return NULL;
    }
    if(seg->size >= size) {
        return seg;
    }
    Segment* max = NULL;
    while(seg) {
        if(seg->start+seg->size >=size && (max == NULL ||seg->size>max->size)) {
            max = seg;
        }
        seg=seg->next;
    }
    return max;
}
//Essaye de trouver un segment libre a l'aide d'une des fonctions plus haut
int find_free_address_strategy(MemoryHandler *handler, int size, int strategy){
    Segment* seg =NULL;
    switch (strategy){
        case 0:
            seg = find_base_fit(handler, size);
            if(!seg){
                return -1;
            }
            return seg->start;
        case 1:
            seg = find_best_fit(handler, size);
            if(!seg){
                return -1;
            }
            return seg->start;
        case 2:
            seg = find_worst_fit(handler, size);
            if(!seg){
                return -1;
            }
            return seg->start;
    }
    return -1;
}

void free_segment(Segment *seg) {
    Segment *temp=seg;
    while(seg) {
        temp=seg;
        seg=seg->next;
        free(temp);
    }
}

void free_memory_handler(MemoryHandler *mem) {
    for(int i=0; i<mem->total_size; i++) {
        free(mem->memory[i]);
    }

    free(mem->memory);
    free_segment(mem->free_list);
    hashmap_destroy(mem->allocated);
    free(mem);
}