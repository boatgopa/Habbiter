#pragma once

#include <string>
#include <unordered_map>

//데이터구조 - 날짜 안에 unordered_map과 완료여부를 저장하는 구조체
struct DailyRecord {
	std::string date;
	std::unordered_map<std::string, bool> habits;
	bool allCompleted = false;
};