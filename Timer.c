#include "Timer.h"

void initTimer(){
    timer = malloc(sizeof(Timer));
}

void setTimer(smol timeLimit){
        timer->timer = get_time_ms(NULL);
        timer->timeLimit = timer->timer + timeLimit;
}

smol timeUp(){
    u64 currentTime = get_time_ms(NULL);
    if(currentTime >= timer->timeLimit){
        return 1;
    }
    return 0;
}