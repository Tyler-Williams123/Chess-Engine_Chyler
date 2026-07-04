#include <stdlib.h>

typedef unsigned long long u64;
typedef unsigned char smol;

u64 State = 83241136785645846570463764134767629435ULL;

void seedRandom(u64 seed){
    State = seed;
}

u64 rand64(){
    State ^= State >> 7;
    State ^= State << 13;
    State ^= State >> 20;

    return State;
}

typedef struct{
    u64 key;
    u64 value;
}node;

typedef struct{
    node* data;
}Dictionary;

void dictInit(Dictionary* d, int size){
    d->data = (node*)malloc(sizeof(node) * size);
}

void put(Dictionary* d, u64 key, u64 value){
    
}