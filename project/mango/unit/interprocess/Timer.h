#pragma once

#include <boost/asio.hpp>
#include <intrin.h>

class Timer {
public:
    Timer() : start_(), end_() {
    }

    void Start() {
        QueryPerformanceCounter(&start_);
    }

    void Stop() {
        QueryPerformanceCounter(&end_);
    }

    double GetElapsedMilliseconds() {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return (end_.QuadPart - start_.QuadPart) * 1000.0 / freq.QuadPart;
    }

    LARGE_INTEGER start() { return start_; };
    LARGE_INTEGER end() { return end_; };

private:
    LARGE_INTEGER start_;
    LARGE_INTEGER end_;
};


class Timer2 {
public:
    Timer2() : start_(), end_() {
    }

    void Start() {
        start_ = __rdtsc();
    }

    void Stop() {
        end_ = __rdtsc();
    }

    double GetElapsedMilliseconds() {
        unsigned long long sleap = end_ - start_;

    }

    unsigned long long start() { return start_; };
    unsigned long long end() { return end_; };

private:
    unsigned long long start_;
    unsigned long long end_;
};



struct data_t
{
    char msg[50];
    Timer delay;
};
