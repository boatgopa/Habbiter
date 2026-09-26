#include "RecordManager.h"

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

//TODO : 아직 날짜가 한군데 빠져있어도 연속으로 True라서 연속 달성으로 COunt가 올라감. 나중에 이를 해결
int RecordManager::Count(const string& date)
{
	FileManager fileManager;

	vector<DailyRecord> records = fileManager.LoadAllRecords();

	int count = 0;

	for (int i = static_cast<int>(records.size()) - 1; i >= 0; --i)
	{
		//미달성시
		if (!records[i].allCompleted)
		{
			break;
		}

		count++;
	}

	return count;
}