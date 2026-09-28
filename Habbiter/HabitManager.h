#pragma once

#include "DailyRecord.h"
#include <string>

//습관을 추가하고 상태를 갱신하는 클래스를 정의하는 헤더 파일
class HabitManager
{
public:
    void AddHabit(DailyRecord& record, const std::string& habitName); // 추가
    void UpdateHabit(DailyRecord& record, const std::string& habitName, bool completed); // 갱신
};