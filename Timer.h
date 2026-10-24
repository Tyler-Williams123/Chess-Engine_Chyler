#pragma once
#include "defs.h"
#include <time.h>

// get_time_ms: milliseconds from a monotonic clock. The argument is unused.
u64 get_time_ms(void* unused);

typedef struct{
    u64 timer;
    u64 timeLimit;
} Timer;

extern Timer *timer;

/**
 * Initializes the timer by allocating memory for it.
 */
void initTimer();

/**
 * Sets the timer with a specified time limit.
 * @param t The timer to set.
 * @param timeLimit The time limit for the timer in milliseconds from the current time.
 */
void setTimer(u64 timeLimit);

/**
 * Checks if the timer has exceeded its time limit.
 * @param t The timer to check.
 * @return 1 if the timer is up, 0 otherwise.
 */
smol timeUp();