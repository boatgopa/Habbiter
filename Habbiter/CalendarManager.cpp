#include "CalendarManager.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace chrono;

std::string CalendarManager::GetToday()
{
    auto now = system_clock::now();
    time_t currentTime = system_clock::to_time_t(now);

    tm localTime{};

    localtime_s(&localTime, &currentTime);

    ostringstream dateStream;

    dateStream << put_time(&localTime, "%Y-%m-%d");
    return dateStream.str();
}