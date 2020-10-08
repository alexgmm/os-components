#pragma once

#define EXECUTION_TIME 30

#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

std::chrono::_V2::system_clock::time_point start;

void startTimeCounting()
{
    start = high_resolution_clock::now();
}

float getElapsedTime()
{
    milliseconds elapsedTime = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    return stod(to_string(elapsedTime.count()));
}

bool isTimeOver()
{
    return ((getElapsedTime() / 1000) >= EXECUTION_TIME);
}
