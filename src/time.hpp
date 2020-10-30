#pragma once

#define EXECUTION_TIME 5

#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

uint64_t start;

uint64_t timeSinceEpochMillisec()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void startTimeCounting()
{
    start = timeSinceEpochMillisec();
}

uint64_t getElapsedTime()
{
    return (timeSinceEpochMillisec() - start) / 1000;
}

bool isTimeOver()
{
    return getElapsedTime() >= EXECUTION_TIME;
}
