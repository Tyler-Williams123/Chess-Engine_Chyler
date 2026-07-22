#include <stdlib.h>
#include "defs.h"

void seedRandom(u64 seed){
    State = seed;
}

u64 rand64(){
    State ^= State >> 7;
    State ^= State << 13;
    State ^= State >> 17;

    return State;
}
// Random

// Dictionary
void dictInit(Dictionary* d, smol size){ //size is exponent 2^n size is n
    d->size = 1 << size;
    d->data = (node*)malloc(sizeof(node) * d->size);
    memset(d->data, 0, sizeof(node) * d->size);
}

u64 dictHash(Dictionary* d, u64 key){
    return key & (d->size - 1);
}

TranspositionTableItem get(Dictionary* d, u64 key){
    u64 hash = dictHash(d, key);
    if(d->data[hash].key == key){
        return d->data[hash].tableItem;
    }
    TranspositionTableItem notFound = {.depth = -1, .bestMove = NO_MOVE, .score = 0};
    return notFound;
}

void put(Dictionary* d, u64 key, TranspositionTableItem item){
    u64 hash = dictHash(d, key);
    if(get(d, key).depth <= item.depth){
        d->data[hash].key = key;
        d->data[hash].tableItem = item;
    }
}
// Dictionary