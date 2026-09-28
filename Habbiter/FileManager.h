#pragma once

#include "Habit.h"
#include "DailyRecord.h"

#include <string>
#include <vector>

// CSV 파일의 저장 및 불러오기를 담당하는 클래스
class FileManager
{
public:
	// 습관 목록을 CSV 파일로 저장하고 불러오는 함수
    bool SaveHabits(const std::vector<Habit>& habits);
    std::vector<Habit> LoadHabits();

	// DailyRecord를 CSV 파일로 저장하고 불러오는 함수
    bool SaveRecord(const DailyRecord& record);
    DailyRecord LoadRecord(const std::string& date);

    // 전체 기록 불러오기 ( Count용)
    std::vector<DailyRecord> LoadAllRecords();

	// 프로그램 실행 시 오늘 날짜의 기록이 없으면 생성하는 함수
    DailyRecord CreateTodayRecord(
        const std::string& date,
        const std::vector<Habit>& habits
    );

    // 조건문
    bool HasRecord(const std::string& date);

    //TODO: 만든 습관 목록을 삭제하는 기능도 만들수도?
};