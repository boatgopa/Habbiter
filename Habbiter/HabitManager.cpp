#include "HabitManager.h"

using namespace std;

//습관을 추가하고 상태를 갱신하는 클래스
void HabitManager::AddHabit(DailyRecord& record, const string& habitName)  // 추가
{
	record.habits[habitName] = false;
}

void HabitManager::UpdateHabit(DailyRecord& record, const string& habitName, bool completed) // 갱신
{
	record.habits[habitName] = completed;
}