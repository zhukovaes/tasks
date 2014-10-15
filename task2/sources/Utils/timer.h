/**
 * @file: types.h 
 * Definition of timer utility
 */
/*
 * Utils library
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once

#ifndef TIMER_H
#define TIMER_H

#include <ctime>

namespace Utils
{

/** 
 * Timer utility. Used for measuring intervals. 
 */
class Timer
{
public:
    /**
     * Constructor. Starts off the timer if 'true' is passed for start_now parameter
     */
    inline Timer( bool start_now = false);
    inline void start();        // < Start the timer
    inline Float elapsed();     //  < Return elapsed time in seconds
    inline UInt32 elapsedUSec();// < Return elapsed time in microseconds
    inline UInt32 elapsedMilliSec();// < Return elapsed time in milliseconds
private:
#ifdef OS_LINUX
    inline UInt64 elapsedInNanoseconds();
    struct timespec start_clock;
#else
    clock_t start_clock;
#endif
};

/**
 * Constructor. Starts off the timer if 'true' is passed for start_now parameter
 */
Timer::Timer( bool start_now)
{
    if ( start_now)
        start();
}

// Start the timer
void Timer::start()
{
#ifdef OS_LINUX
    clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &start_clock);
#else
    start_clock = std::clock();
#endif
}


#ifdef OS_LINUX
inline UInt64 tv2nsec( struct timespec * tv)
{
    return tv->tv_sec * 1E9 + tv->tv_nsec;
}

UInt64 Timer::elapsedInNanoseconds()
{
    struct timespec current_clock;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &current_clock);
    
    UInt64 start_nsec = tv2nsec( &start_clock);
    UInt64 cur_nsec = tv2nsec( &current_clock);
    return cur_nsec - start_nsec;
}
#endif

// Elapsed time in microseconds
UInt32 Timer::elapsedUSec()
{
#ifdef OS_LINUX
    return (UInt32)(((double)elapsedInNanoseconds()) / 1000); 
#else    
    clock_t delta = std::clock() - start_clock;
    return (UInt32)((1000000 * (double)delta)/CLOCKS_PER_SEC);
#endif
}

// Elapsed time in milliseconds
UInt32 Timer::elapsedMilliSec()
{
#ifdef OS_LINUX
    struct timespec current_clock;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &current_clock);
    return (UInt32)(((double)elapsedInNanoseconds()) / 1000000); 
#else    
    clock_t delta = std::clock() - start_clock;
    return (UInt32)((1000 * (double)delta)/CLOCKS_PER_SEC);
#endif
}

Float Timer::elapsed()
{
#ifdef OS_LINUX
    struct timespec current_clock;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &current_clock);
    return current_clock.tv_sec - start_clock.tv_sec; 
#else    
    clock_t delta = clock() - start_clock;
    return (float)delta/CLOCKS_PER_SEC;
#endif
}

}
#endif /* TIMER_H */