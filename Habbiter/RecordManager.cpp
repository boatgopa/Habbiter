#include "RecordManager.h"
//날짜 비교용
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

// 해당 날짜의 습관들의 완료 여부를 확인하고 저장하는 클래스
void RecordManager::AllCompleted(DailyRecord& record)
{
	// 습관이 비어있는 초기 상태
	if (record.habits.empty())
	{
		record.allCompleted = false;
		return;
		// TODO : 습관이 비어있을 때의 처리 로직 추가 가능
	}

	// 습관이 하나라도 false인지 확인하는 로직
	for (const auto& habit : record.habits)
	{
		if (!habit.second)
		{
			record.allCompleted = false;
			return;
		}
	}
	// 습관이 모두 true일 경우 
	record.allCompleted = true;
}

int RecordManager::Count(const std::string& date)
{
    FileManager fileManager;

    std::vector<DailyRecord> records =
        fileManager.LoadAllRecords();

    std::string currentDate = date;

    int count = 0;

    while (true)
    {
        bool found = false;

        for (const auto& record : records)
        {
            if (record.date == currentDate)
            {
                found = true;

                if (!record.allCompleted)
                {
                    return count;
                }

                count++;
                break;
            }
        }

        // 해당 날짜의 기록이 없으면 연속이 끊김
        if (!found)
        {
            break;
        }

        // 하루 전 날짜로 이동
        currentDate = GetYesterday(currentDate);

        if (currentDate.empty())
        {
            break;
        }
    }

    return count;
}

//어제거랑 오늘 날짜 비교
std::string RecordManager::GetYesterday(const std::string& date)
{
    std::tm time{};

    std::istringstream stream(date);

    stream >> std::get_time(&time, "%Y-%m-%d");

    if (stream.fail())
    {
        return "";
    }

    time.tm_hour = 12;

    std::time_t timeValue = std::mktime(&time);

    timeValue -= 24 * 60 * 60;

    std::tm previousTime{};

    localtime_s(&previousTime, &timeValue);

    std::ostringstream result;

    result << std::put_time(
        &previousTime,
        "%Y-%m-%d"
    );

    return result.str();
}