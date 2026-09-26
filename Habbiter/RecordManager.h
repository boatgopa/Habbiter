#pragma once

#include "DailyRecord.h"
#include "FileManager.h"

#include <vector>

using namespace std;

// DailyRecord 구조체를 통하여 해당 날짜의 습관의 완료 여부를 확인하고 저장하는 클래스를 정의하는 헤더 파일
class RecordManager 
{
public:
	void AllCompleted(DailyRecord& record);
	int Count(const string& date);
};