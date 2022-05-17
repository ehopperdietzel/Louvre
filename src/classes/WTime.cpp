#include <WTime.h>
#include <time.h>

using namespace Wpp;

UInt32 Wpp::WTime::ms()
{
    timespec endTime;
    clock_gettime(CLOCK_MONOTONIC_RAW, &endTime);
    return endTime.tv_sec*1000 + endTime.tv_nsec/1000000;
}

timespec WTime::us()
{
    timespec endTime;
    clock_gettime(CLOCK_MONOTONIC_RAW, &endTime);
    return endTime;
}
