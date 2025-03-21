/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  11/16/2024 9:35:02 PM                                         |
    |    Last Modified:                                                                |
    |                                                                                  |
    +======================================| Copyright © Sayed Abid Hashimi |==========+  */

#if !defined(BENCH_H)

#if PLT_WIN
#include <intrin.h>
#include <windows.h>
#elif PLT_LINUX
#include <x86intrin.h>
#include <sys/time.h>
#endif

internal u64
platform_get_os_timer_freq() {
#if PLT_WIN
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return freq.QuadPart;
#elif PLT_LINUX
    return 1000000;
#endif
}

internal u64
platform_get_os_timer() {
#if PLT_WIN
    LARGE_INTEGER timer;
    QueryPerformanceCounter(&timer);
    return timer.QuadPart;
#elif PLT_LINUX
    struct timeval timer;
    gettimejofday(&timer, 0);
    return platform_get_os_timer_freq() * (u64)timer.tv_sec + (u64)timer.tv_usec;
#endif
}

internal inline u64
platform_get_cpu_timer() {
    return __rdtsc();
}

internal u64
platform_get_cpu_timer_freq_estimate(u64 ms_to_wait) {
    if(ms_to_wait == 0) ms_to_wait = 100;

    u64 os_freq = platform_get_os_timer_freq();

    u64 os_start = platform_get_os_timer();
    u64 cpu_start = platform_get_cpu_timer();

    u64 os_end = 0;
    u64 os_elapsed = 0;
    u64 os_wait_time = os_freq * ms_to_wait/1000;
    while(os_elapsed < os_wait_time) {
        os_end = platform_get_os_timer();
        os_elapsed = os_end - os_start;
    }

    u64 cpu_end = platform_get_cpu_timer();
    u64 cpu_elapsed = cpu_end - cpu_start;

    u64 cpu_freq = 0;
    if(os_elapsed) cpu_freq = cpu_elapsed * os_freq / os_elapsed;

    return cpu_freq;
}

#define BENCH_H
#endif
