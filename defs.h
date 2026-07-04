#include <stdlib.h>

typedef unsigned long long u64;
typedef unsigned char smol;

// Random
u64 State = 64134767629435ULL;

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
typedef struct{
    u64 key;
    int value;
}node;

typedef struct{
    node* data;
    u64 size;
}Dictionary;

void dictInit(Dictionary* d, smol size){ //size is exponent 2^n size is n
    d->size = 1 << size;
    d->data = (node*)malloc(sizeof(node) * d->size);
}

u64 dictHash(Dictionary* d, u64 key){
    return key & (d->size - 1);
}

void put(Dictionary* d, u64 key, int value){
    d->data[dictHash(d, key)].key = key;
    d->data[dictHash(d, key)].value = value;
}

int get(Dictionary* d, u64 key){
    return d->data[dictHash(d, key)].value;
}
// Dictionary