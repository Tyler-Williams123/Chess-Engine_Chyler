#include "Timer.h"

void initTimer(){
    timer = malloc(sizeof(Timer));
}

void setTimer(u64 timeLimit){
        if(timer == NULL){
            initTimer();
        }
        if(timeLimit == -1){
            timer->timeLimit = -1;
            return;
        }
        timer->timer = get_time_ms(NULL);
        timer->timeLimit = timer->timer + timeLimit;
}

smol timeUp(){
    if(timer->timeLimit == -1){
        return 0;
    }

    u64 currentTime = get_time_ms(NULL);
    if(currentTime >= timer->timeLimit){
        return 1;
    }
    return 0;
}