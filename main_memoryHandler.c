#include "hashmap.h"
#include "memoryHandler.h"
#include <stdio.h>
#include <assert.h>

int main(){
    MemoryHandler *memo = memory_init(1000);
    Segment *border1, *border2, *border3, *parcours;
    border1=(Segment *)malloc(sizeof(Segment));
    border1->next=NULL;
    border1->size=100;
    border1->start=0;

    border2=(Segment *)malloc(sizeof(Segment));
    border2->next=NULL;
    border2->size=200;
    border2->start=800;

    border3=(Segment *)malloc(sizeof(Segment));
    border3->next=NULL;
    border3->size=200;
    border3->start=300;
    parcours=memo->free_list;
    
    create_segment(memo,"border1", border1->start, border1->size);
    create_segment(memo,"border2", border2->start, border2->size);
    create_segment(memo,"border3", border3->start, border3->size);
    
    parcours=memo->free_list;
    assert(parcours->start==border1->start+border1->size);
    assert(parcours->start+parcours->size==border3->start);
    parcours=parcours->next;
    assert(parcours->start==border3->start+border3->size);
    assert(parcours->start+parcours->size==border2->start);
    return 0;
}