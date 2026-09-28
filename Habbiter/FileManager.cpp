#include "FileManager.h"

#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

//습관 저장
bool FileManager::SaveHabits(const vector<Habit>& habits)
{
	ofstream file("habits.csv");

	if (!file.is_open())
	{
		return false;
	}

	file << "name,date\n";

	for (const auto& habit : habits)
	{
		file << habit.name << "," << habit.date << "\n";
	}

	file.close();

	return true;
}

//습관 파일 불러오기
vector<Habit> FileManager::LoadHabits()
{
	vector<Habit> habits;
	ifstream file("Habits.csv");

	if (!file.is_open())
	{
		return habits;
		// TODO : 파일 열기 실패 처리 로직 추가, 메시지 출력은 하는데 파일 생성은 굳이?
	}

	string line;

	getline(file, line); // 첫 줄 헤더 건너뛰기

	while (getline(file, line))
	{
		stringstream ss(line);
		Habit habit;
		
		getline(ss, habit.name, ',');
		getline(ss, habit.date, ',');

		habits.push_back(habit);
	}
	file.close();
	return habits;
}

//기록 저장하기
bool FileManager::SaveRecord(const DailyRecord& record)
{
	ifstream inputFile("Records.csv");

	vector<DailyRecord> records;

	if (inputFile.is_open())
	{
		string line;

		// 헤더 건너뛰기
		getline(inputFile, line);

		while (getline(inputFile, line))
		{
			stringstream ss(line);

			string recordDate;
			string allCompleted;
			string habits;

			getline(ss, recordDate, ',');
			getline(ss, allCompleted, ',');
			getline(ss, habits);

			DailyRecord loadedRecord;

			loadedRecord.date = recordDate;
			loadedRecord.allCompleted =
				(allCompleted == "true");

			stringstream habitStream(habits);
			string habitData;

			while (getline(habitStream, habitData, '|'))
			{
				stringstream habitPair(habitData);

				string habitName;
				string completed;

				getline(habitPair, habitName, '=');
				getline(habitPair, completed);

				loadedRecord.habits[habitName] =
					(completed == "true");
			}

			records.push_back(loadedRecord);
		}

		inputFile.close();
	}

	// 같은 날짜의 기록 찾기
	bool found = false;

	for (auto& savedRecord : records)
	{
		if (savedRecord.date == record.date)
		{
			savedRecord = record;
			found = true;
			break;
		}
	}

	// 같은 날짜가 없으면 새 기록 추가
	if (!found)
	{
		records.push_back(record);
	}

	ofstream file("Records.csv");

	if (!file.is_open())
	{
		return false;
	}

	file << "date,allCompleted,habits\n";

	for (const auto& savedRecord : records)
	{
		file << savedRecord.date << ",";
		file << (savedRecord.allCompleted ? "true" : "false") << ",";

		bool first = true;

		for (const auto& habit : savedRecord.habits)
		{
			if (!first)
			{
				file << "|";
			}

			file << habit.first << "=";
			file << (habit.second ? "true" : "false");

			first = false;
		}

		file << "\n";
	}

	file.close();

	return true;
}

//기록 불러오기
DailyRecord FileManager::LoadRecord(const string& date)
{
	DailyRecord record;
	record.date = date;
	record.allCompleted = false;

	ifstream file("Records.csv");

	if (!file.is_open()) {
		return record;
		//TODO: 오류 메시지 출력, 파일 생성은 굳이?
	}

	string line;

	//첫번째 라인 헤더 건너뛰기
	getline(file, line);

	while (getline(file, line))
	{
		stringstream ss(line);

		string recordDate;
		string allCompleted;
		string habits;

		getline(ss, recordDate, ',');
		getline(ss, allCompleted, ',');
		getline(ss, habits);

		if (recordDate != date)
		{
			continue;
		}

		record.allCompleted = (allCompleted == "true");

		stringstream habitStream(habits);
		string habitData;

		while (getline(habitStream, habitData, '|'))
		{
			stringstream habits(habitData);

			string habitName;
			string completed;

			getline(habits, habitName, '=');
			getline(habits, completed);

			record.habits[habitName] = (completed == "true");
		}
		break;
	}
	file.close();

	return record;
}

vector<DailyRecord> FileManager::LoadAllRecords()
{
	vector<DailyRecord> records;

	ifstream file("Records.csv");

	if (!file.is_open())
	{
		return records;
		//TODO : 얘는 파일 생성 할수도? 일단 보류
	}

	string line;

	//첫번째 줄 헤더는 건너뛰기
	getline(file, line);

	while (getline(file, line))
	{
		stringstream ss(line);

		string recordDate;
		string allCompleted;
		string habits;

		getline(ss, recordDate, ',');
		getline(ss, allCompleted, ',');
		getline(ss, habits);

		DailyRecord record;

		record.date = recordDate;
		record.allCompleted = (allCompleted == "true");

		stringstream habitStream(habits);
		string habitData;

		while (getline(habitStream, habitData, '|'))
		{
			stringstream habits(habitData);

			string habitName;
			string completed;

			getline(habits, habitName, '=');
			getline(habits, completed);

			record.habits[habitName] = (completed == "true");
		}

		records.push_back(record);
	}

	file.close();
	return records;
}

DailyRecord FileManager::CreateTodayRecord(
	const std::string& date,
	const std::vector<Habit>& habits)
{
	DailyRecord record;

	record.date = date;
	record.allCompleted = false;

	for (const auto& habit : habits)
	{
		if (habit.date <= date)
		{
			record.habits[habit.name] = false;
		}
	}

	SaveRecord(record);

	return record;
}

bool FileManager::HasRecord(const std::string& date)
{
	ifstream file("Records.csv");

	if (!file.is_open())
	{
		return false;
	}

	string line;

	// 헤더 건너뛰기
	getline(file, line);

	while (getline(file, line))
	{
		stringstream ss(line);

		string recordDate;

		getline(ss, recordDate, ',');

		if (recordDate == date)
		{
			file.close();
			return true;
		}
	}

	file.close();

	return false;
}