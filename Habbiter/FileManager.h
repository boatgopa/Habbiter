#pragma once

#include "Habit.h"
#include "DailyRecord.h"

#include <string>
#include <vector>

using namespace std;

// CSV 파일의 저장 및 불러오기를 담당하는 클래스
class FileManager
{
public:
	// 습관 목록을 CSV 파일로 저장하고 불러오는 함수
    void SaveHabits(const vector<Habit>& habits);
    vector<Habit> LoadHabits();

	// DailyRecord를 CSV 파일로 저장하고 불러오는 함수
    void SaveRecord(const DailyRecord& record);
    DailyRecord LoadRecord(const string& date);

    // 전체 기록 불러오기 ( Count용)
    vector<DailyRecord> LoadAllRecords();
    //TODO: 만든 습관 목록을 삭제하는 기능도 만들수도?
};