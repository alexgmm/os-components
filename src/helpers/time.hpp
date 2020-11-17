#pragma once

#define EXECUTION_TIME 30

#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

uint64_t start;

uint64_t timeSinceEpoch()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void startTimeCounting()
{
    start = timeSinceEpoch();
}

uint64_t getElapsedTime()
{
    return (timeSinceEpoch() - start) / 1000;
}

bool isTimeOver()
{
    return getElapsedTime() >= EXECUTION_TIME;
}
