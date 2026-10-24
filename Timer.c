#include "Timer.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

Timer *timer = NULL; // Timer.h only declares it (extern)

u64 get_time_ms(void* unused){
    (void)unused;
    return (u64)GetTickCount64(); // monotonic, milliseconds since boot
}

void initTimer(){
    timer = malloc(sizeof(Timer));
    timer->timer = 0;
    timer->timeLimit = (u64)-1;
}

void setTimer(u64 timeLimit){
        if(timer == NULL){
            initTimer();
        }
        if(timeLimit == (u64)-1){
            timer->timeLimit = (u64)-1;
            return;
        }
        timer->timer = get_time_ms(NULL);
        timer->timeLimit = timer->timer + timeLimit;
}

smol timeUp(){
    if(timer == NULL || timer->timeLimit == (u64)-1){
        return 0;
    }

    u64 currentTime = get_time_ms(NULL);
    if(currentTime >= timer->timeLimit){
        return 1;
    }
    return 0;
}