// Habbiter.cpp : 애플리케이션에 대한 진입점을 정의합니다.
#pragma comment(lib, "comctl32.lib")

#include "framework.h"
#include "Habbiter.h"
#include "FileManager.h"
#include "HabitManager.h"
#include "RecordManager.h"
#include "CalendarManager.h"
#include "EH.h"

#include <algorithm>
#include <commctrl.h>
#include <windows.h>

#define MAX_LOADSTRING 100

using namespace std;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND hMonthCalendar; // 달력
HWND hHabitInput; // 습관 입력창
HWND hHabitButton; // 습관 추가 버튼
HWND hSuccess; // 연속 달성 변수 
HWND hCalendarPanel; // 모서리
HWND hSuccessPanel; // 모서리
HWND hTitlePanel; // 모서리
HWND hSelectedDatePanel; // 선택한 날짜 표기
DailyRecord currentRecord; // 기록 수정용 변수
vector<string> currentHabitNames; // 선택된 기록 이름 변수
vector<HWND> hHabitNames; // 습관 이름 변수
vector<HWND> hHabitButtons; // 습관 상태 변경 버튼
HWND hHabitListPanel; // 습관 목록 영역
HWND hTitle; // 타이틀
HFONT hTitleFont; // 타이틀 글꼴
HFONT hSuccessFont; // 연속 달성 글꼴
HBRUSH hGreenBrush; // 초록색 브러쉬
HBRUSH hRedBrush; // 빨간색 브러쉬
HBRUSH hBackgroundBrush; // 검은색 배경 브러쉬
HBRUSH hAreaBrush; // 회색 영역 브러쉬

constexpr int HABIT_ID = 2000; // 버튼 Id 상수 기본값

// 좌표 상수
constexpr int HABIT_LIST_TOP = 85;
constexpr int HABIT_LIST_HEIGHT = 300;
constexpr int HABIT_ROW_HEIGHT = 45;


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
wstring Utf8ToWide(const string& str);

void CreateHabitList(HWND hWnd);
void UpdateHabitList(HWND hWnd);
void HabitButton(HWND hWnd, int buttonId);

/////
// 테두리 함수들
/////
LRESULT CALLBACK HabitListPanelProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);

LRESULT CALLBACK CalendarPanelProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);

LRESULT CALLBACK SuccessPanelProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);

LRESULT CALLBACK TitlePanelProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);

LRESULT CALLBACK SelectedDatePanelProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);

//둘데 없는 함수들 여기 두기

// 습관 목록 UI 생성
void CreateHabitList(HWND hWnd)
{
    // 기존 습관 UI 제거
    for (HWND hName : hHabitNames)
    {
        DestroyWindow(hName);
    }

    for (HWND hButton : hHabitButtons)
    {
        DestroyWindow(hButton);
    }

    hHabitNames.clear();
    hHabitButtons.clear();
    currentHabitNames.clear();

    int y = 10;
    int buttonId = HABIT_ID;

    for (const auto& habit : currentRecord.habits)
    {
        currentHabitNames.push_back(habit.first);

        HWND hName = CreateWindowExW(
            0,
            L"STATIC",
            Utf8ToWide(habit.first).c_str(),
            WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
            20,
            y,
            270,
            40,
            hHabitListPanel,
            nullptr,
            hInst,
            nullptr
        );

        HWND hStatusButton = CreateWindowExW(
            0,
            L"BUTTON",
            habit.second ? L"O" : L"X",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            310,
            y,
            60,
            36,
            hHabitListPanel,
            (HMENU)buttonId,
            hInst,
            nullptr
        );

        hHabitNames.push_back(hName);
        hHabitButtons.push_back(hStatusButton);

        y += HABIT_ROW_HEIGHT;
        buttonId++;
    }
}

// 습관 목록 UI 갱신
void UpdateHabitList(HWND hWnd)
{
    int y = 10;

    for (size_t i = 0; i < hHabitNames.size(); i++)
    {
        int habitY =
            y + static_cast<int>(i) * HABIT_ROW_HEIGHT;

        HWND hName = hHabitNames[i];
        HWND hButton = hHabitButtons[i];

        {
            ShowWindow(hName, SW_SHOW);
            ShowWindow(hButton, SW_SHOW);

            MoveWindow(
                hName,
                10,
                habitY,
                280,
                40,
                TRUE
            );

            InvalidateRect(
                hName,
                nullptr,
                TRUE
            );

            MoveWindow(
                hButton,
                320,
                habitY,
                70,
                40,
                TRUE
            );


            InvalidateRect(
                hButton,
                nullptr,
                TRUE
            );

        }
    }
}

// 습관 상태 버튼 처리
void HabitButton(HWND hWnd, int buttonId)
{
    int selectedIndex =
        buttonId - HABIT_ID;

    // 잘못된 버튼 ID 방지
    if (selectedIndex < 0 ||
        selectedIndex >=
        static_cast<int>(currentHabitNames.size()))
    {
        return;
    }

    std::string habitName =
        currentHabitNames[selectedIndex];

    HabitManager habitManager;
    RecordManager recordManager;
    FileManager fileManager;
    CalendarManager calendarManager;

    // 현재 상태 저장
    bool currentState =
        currentRecord.habits[habitName];

    // O ↔ X 변경
    habitManager.UpdateHabit(
        currentRecord,
        habitName,
        !currentState
    );

    // 전체 완료 여부 갱신
    recordManager.AllCompleted(
        currentRecord
    );

    // 기록 저장
    if (!fileManager.SaveRecord(currentRecord))
    {
        // 저장 실패 시 원래 상태로 복구
        currentRecord.habits[habitName] =
            currentState;

        recordManager.AllCompleted(
            currentRecord
        );

        EH::ShowSaveError(hWnd);

        return;
    }

    // 오늘 날짜라면 연속 완료 갱신
    if (currentRecord.date ==
        calendarManager.GetToday())
    {
        int straight =
            recordManager.Count(
                calendarManager.GetToday()
            );

        std::wstring straightText =
            L"연속 완료: " +
            std::to_wstring(straight) +
            L"일🔥🔥🔥";

        SetWindowTextW(
            hSuccess,
            straightText.c_str()
        );
    }


    // 화면 갱신
    HWND hButton = hHabitButtons[selectedIndex];

    SetWindowTextW(
        hButton,
        currentRecord.habits[habitName] ? L"O" : L"X"
    );

    InvalidateRect(
        hButton,
        nullptr,
        TRUE
    );

    UpdateWindow(hButton);

    InvalidateRect(
        hSelectedDatePanel,
        nullptr,
        TRUE
    );

    UpdateWindow(hSelectedDatePanel);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
    INITCOMMONCONTROLSEX icex{};

    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_DATE_CLASSES;

    InitCommonControlsEx(&icex);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HABBITER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HABBITER));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HABBITER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HABBITER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));


    //
    // 테두리 클래스들
    //
    RegisterClassExW(&wcex);

    WNDCLASSEXW panelClass = {};

    panelClass.cbSize = sizeof(WNDCLASSEXW);
    panelClass.lpfnWndProc = HabitListPanelProc;
    panelClass.hInstance = hInstance;
    panelClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    panelClass.hbrBackground = nullptr;
    panelClass.lpszClassName = L"HabitListPanel";

    RegisterClassExW(&panelClass);

    WNDCLASSEXW calendarClass = {};

    calendarClass.cbSize = sizeof(WNDCLASSEXW);
    calendarClass.lpfnWndProc = CalendarPanelProc;
    calendarClass.hInstance = hInstance;
    calendarClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    calendarClass.hbrBackground = nullptr;
    calendarClass.lpszClassName = L"CalendarPanel";

    RegisterClassExW(&calendarClass);

    WNDCLASSEXW successPanelClass = {};
    successPanelClass.cbSize = sizeof(WNDCLASSEXW);
    successPanelClass.lpfnWndProc = SuccessPanelProc;
    successPanelClass.hInstance = hInstance;
    successPanelClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    successPanelClass.hbrBackground = nullptr;
    successPanelClass.lpszClassName = L"SuccessPanel";

    RegisterClassExW(&successPanelClass);

    WNDCLASSEXW titleClass = {};
    titleClass.cbSize = sizeof(WNDCLASSEXW);
    titleClass.lpfnWndProc = TitlePanelProc;
    titleClass.hInstance = hInstance;
    titleClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    titleClass.hbrBackground = nullptr;
    titleClass.lpszClassName = L"TitlePanel";

    RegisterClassExW(&titleClass);

    WNDCLASSEXW wcDatePanel = {};
    wcDatePanel.cbSize = sizeof(WNDCLASSEX);
    wcDatePanel.lpfnWndProc = SelectedDatePanelProc;
    wcDatePanel.hInstance = hInstance;
    wcDatePanel.hbrBackground = nullptr;
    wcDatePanel.lpszClassName = L"SelectedDatePanel";

    RegisterClassExW(&wcDatePanel);


    return TRUE;
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   hGreenBrush = CreateSolidBrush(RGB(100, 200, 100));
   hRedBrush = CreateSolidBrush(RGB(240, 100, 100));
   hAreaBrush = CreateSolidBrush(RGB(245, 247, 250));
   hBackgroundBrush = CreateSolidBrush(RGB(144, 238, 144));

   HWND hWnd = CreateWindowW(
       szWindowClass,
       szTitle,
       WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
       CW_USEDEFAULT,
       CW_USEDEFAULT,
       900,
       650,
       nullptr,
       nullptr,
       hInstance,
       nullptr);

   if (!hWnd)
   {
       return FALSE;
   }
   // 테두리
   hTitlePanel = CreateWindowExW(
       0,
       L"TitlePanel",
       L"",
       WS_CHILD | WS_VISIBLE,
       30,
       20,
       200,
       50,
       hWnd,
       nullptr,
       hInst,
       nullptr
   );
   // 타이틀
   hTitle = CreateWindowExW(
       0,
       L"STATIC",
       L"나의 습관 일지",
       WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
       5,
       5,
       190,
       40,
       hTitlePanel,
       nullptr,
       hInst,
       nullptr
   );
   // 타이틀 글꼴
   hTitleFont = CreateFontW(
       32,
       0,
       0,
       0,
       FW_BOLD,
       FALSE,
       FALSE,
       FALSE,
       DEFAULT_CHARSET,
       OUT_DEFAULT_PRECIS,
       CLIP_DEFAULT_PRECIS,
       DEFAULT_QUALITY,
       DEFAULT_PITCH | FF_SWISS,
       L"맑은 고딕"
   );

   SendMessage(
       hTitle,
       WM_SETFONT,
       (WPARAM)hTitleFont,
       TRUE
   );
   //테두리
   hCalendarPanel = CreateWindowExW(
       0,
       L"CalendarPanel",
       L"",
       WS_CHILD | WS_VISIBLE,
       30,
       85,
       350,
       340,
       hWnd,
       nullptr,
       hInst,
       nullptr
   );

   // 달력 생성
   hMonthCalendar = CreateWindowExW(0, MONTHCAL_CLASSW, L"", WS_CHILD | WS_VISIBLE | MCS_NOTODAYCIRCLE,
       10, 30, 330, 280, hCalendarPanel, nullptr, hInstance, nullptr);

   // 습관 입력창 생성
   hHabitInput = CreateWindowExW(
       WS_EX_CLIENTEDGE,
       L"EDIT",
       L"",
       WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_CENTER,
       430, 460, 280, 75,
       hWnd,
       nullptr,
       hInstance,
       nullptr
   );

   // 습관 추가 버튼 생성
   hHabitButton = CreateWindowExW(
       0,
       L"BUTTON",
       L"습관 추가",
       WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
       720, 460, 120, 35,
       hWnd,
       (HMENU)1002,
       hInstance,
       nullptr
   );
   // 습관 삭제 버튼 생성
   HWND hHabitDeleteButton = CreateWindowExW(
       0,
       L"BUTTON",
       L"습관 삭제",
       WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
       720, 500, 120, 35,
       hWnd,
       (HMENU)1003,
       hInstance,
       nullptr
   );
   // 테두리
   hSuccessPanel = CreateWindowExW(
       0,
       L"SuccessPanel",
       L"",
       WS_CHILD | WS_VISIBLE,
       30, 450, 350, 85,
       hWnd,
       nullptr,
       hInst,
       nullptr
   );
   // 연속 달성
   hSuccess = CreateWindowExW(
       0,
       L"STATIC",
       L"연속 완료: 0일!!!",
       WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
       45, 465, 320, 55,
       hWnd,
       nullptr,
       hInstance,
       nullptr
   );
   // 글꼴
   hSuccessFont = CreateFontW(
       28,
       0,
       0,
       0,
       FW_BOLD,
       FALSE,
       FALSE,
       FALSE,
       DEFAULT_CHARSET,
       OUT_DEFAULT_PRECIS,
       CLIP_DEFAULT_PRECIS,
       DEFAULT_QUALITY,
       DEFAULT_PITCH | FF_SWISS,
       L"맑은 고딕"
   );

   SendMessageW(
       hSuccess,
       WM_SETFONT,
       (WPARAM)hSuccessFont,
       TRUE
   );


   CalendarManager calendarManager;
   FileManager fileManager;
   ////
   // 오늘 날짜 기록이 없으면 생성, 있으면 불러오기
   ///
   std::string today = calendarManager.GetToday();

   if (!fileManager.HasRecord(today))
   {
       std::vector<Habit> habits =
           fileManager.LoadHabits();

       currentRecord =
           fileManager.CreateTodayRecord(
               today,
               habits
           );
   }
   else
   {
       currentRecord =
           fileManager.LoadRecord(today);
   }

   CreateHabitList(hWnd);
   UpdateHabitList(hWnd);

   // 연속 완료 출력
   RecordManager recordManager;

   int straight =
       recordManager.Count(today);

   std::wstring straightText =
       L"연속 완료: " +
       std::to_wstring(straight) +
       L"일🔥🔥🔥";

   SetWindowTextW(
       hSuccess,
       straightText.c_str()
   );

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// 문자열 변환 함수: UTF-8 → Wide ( 윈도우 api에서 사용하는 문자열 형식)
wstring Utf8ToWide(const string& str)
{
    if (str.empty())
    {
        return L"";
    }

    int sizeNeeded = MultiByteToWideChar(
        CP_UTF8,
        0,
        str.c_str(),
        -1,
        nullptr,
        0
    );

    wstring result(sizeNeeded, L'\0');

    MultiByteToWideChar(
        CP_UTF8,
        0,
        str.c_str(),
        -1,
        &result[0],
        sizeNeeded
    );

    result.resize(sizeNeeded - 1);

    return result;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        
    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;

        RECT rect;
        GetClientRect(hWnd, &rect);

        FillRect(hdc, &rect, hBackgroundBrush);

        return 1;
    }
	// 상태 변경 버튼 색상 변경
    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        HWND hButton = (HWND)lParam;

        for (size_t i = 0; i < hHabitButtons.size(); i++)
        {
            if (hHabitButtons[i] == hButton)
            {
                string habitName = currentHabitNames[i];

                if (currentRecord.habits[habitName])
                {
                    SetBkColor(hdc, RGB(100, 200, 100));
                    return (LRESULT)hGreenBrush;
                }
                else
                {
                    SetBkColor(hdc, RGB(240, 100, 100));
                    return (LRESULT)hRedBrush;
                }
            }
        }

        break;
    }

    // UI 생성
    case WM_CREATE:
    {
        hHabitListPanel = CreateWindowExW(
            0,
            L"HabitListPanel",
            L"",
            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
            430,
            HABIT_LIST_TOP,
            420,
            HABIT_LIST_HEIGHT,
            hWnd,
            nullptr,
            hInst,
            nullptr
        );

        hSelectedDatePanel = CreateWindowExW(
            0,
            L"SelectedDatePanel",
            nullptr,
            WS_CHILD | WS_VISIBLE,
            430,
            400,
            420,
            50,
            hWnd,
            nullptr,
            hInst,
            nullptr
        );

        HRGN hRegion = CreateRectRgn(
            0,
            0,
            420,
            HABIT_LIST_HEIGHT
        );
            
        SetWindowRgn(
            hHabitListPanel,
            hRegion,
            TRUE
        );

        break;
    }
	// 습관 버튼 처리
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        // 습관 O/X 버튼인지 먼저 확인
        if (wmId >= HABIT_ID &&
            wmId <
            HABIT_ID +
            static_cast<int>(
                currentHabitNames.size()
                ))
        {
            HabitButton(
                hWnd,
                wmId
            );

            break;
        }

        // 일반 명령 처리
        switch (wmId)
        {
        // About
        case IDM_ABOUT:

            DialogBox(
                hInst,
                MAKEINTRESOURCE(IDD_ABOUTBOX),
                hWnd,
                About
            );

            break;


        // 프로그램 종료
        case IDM_EXIT:

            DestroyWindow(hWnd);

            break;

        // 습관 추가
        case 1002:
        {
            CalendarManager calendarManager;
            FileManager fileManager;


            // 오늘 날짜 가져오기
            std::string today =
                calendarManager.GetToday();


            // 오늘 날짜가 아니면 추가 불가능
            if (currentRecord.date != today)
            {
                MessageBoxW(
                    hWnd,
                    L"습관 추가는 오늘로 이동하신 후 가능합니다.",
                    L"알림",
                    MB_OK
                );

                break;
            }


            // 입력값 가져오기
            wchar_t input[256];

            GetWindowTextW(
                hHabitInput,
                input,
                256
            );

            std::wstring wideHabitName(input);


            // 빈 입력 검사
            if (wideHabitName.empty())
            {
                EH::ShowEmptyInputError(hWnd);

                break;
            }


            // 유니코드 → UTF-8 변환
            int size = WideCharToMultiByte(
                CP_UTF8,
                0,
                wideHabitName.c_str(),
                -1,
                nullptr,
                0,
                nullptr,
                nullptr
            );


            if (size <= 0)
            {
                EH::ShowEmptyInputError(hWnd);

                break;
            }


            std::string habitName(
                size,
                '\0'
            );


            WideCharToMultiByte(
                CP_UTF8,
                0,
                wideHabitName.c_str(),
                -1,
                &habitName[0],
                size,
                nullptr,
                nullptr
            );


            // NULL 문자 제거
            habitName.pop_back();


            // 기존 습관 불러오기
            std::vector<Habit> habits =
                fileManager.LoadHabits();

            bool alreadyExists = false;


            for (const auto& habit : habits)
            {
                if (habit.name == habitName)
                {
                    alreadyExists = true;
                    break;
                }
            }


            // 중복 검사
            if (alreadyExists)
            {
                EH::ShowDuplicateHabitError(hWnd);

                break;
            }


            // 새로운 습관 생성
            Habit newHabit;

            newHabit.name = habitName;
            newHabit.date = today;

            habits.push_back(newHabit);

            // Habits.csv 저장
            if (!fileManager.SaveHabits(habits))
            {
                EH::ShowSaveError(hWnd);

                break;
            }


            // 오늘 기록에 습관 추가
            currentRecord.habits[habitName] = false;
            currentRecord.allCompleted = false;



            // Records.csv 저장
            if (!fileManager.SaveRecord(currentRecord))
            {
                // 메모리 복구
                currentRecord.habits.erase(
                    habitName
                );


                RecordManager recordManager;

                recordManager.AllCompleted(
                    currentRecord
                );


                // Habits.csv도 원래 상태로 복구
                habits.pop_back();

                fileManager.SaveHabits(habits);


                EH::ShowSaveError(hWnd);

                break;
            }

            RecordManager recordManager;

            // 오늘 날짜라면 연속 완료 갱신
            if (currentRecord.date ==
                calendarManager.GetToday())
            {
                int straight =
                    recordManager.Count(
                        calendarManager.GetToday()
                    );

                std::wstring straightText =
                    L"연속 완료: " +
                    std::to_wstring(straight) +
                    L"일🔥🔥🔥";

                SetWindowTextW(
                    hSuccess,
                    straightText.c_str()
                );
            }


            recordManager.AllCompleted(currentRecord);

            InvalidateRect(hSelectedDatePanel, nullptr, TRUE);
            UpdateWindow(hSelectedDatePanel);

            // 화면 갱신
			CreateHabitList(hWnd);
            UpdateHabitList(hWnd);


            // 입력창 비우기
            SetWindowTextW(
                hHabitInput,
                L""
            );


            // 추가 완료 메시지
            MessageBoxW(
                hWnd,
                L"습관이 추가되었습니다.",
                L"알림",
                MB_OK
            );

            break;
        }

        //case 1002 생성이랑 로직 동일
        case 1003:
        {

            CalendarManager calendarManager;
            FileManager fileManager;

            std::string today =
                calendarManager.GetToday();

            if (currentRecord.date != today)
            {
                MessageBoxW(
                    hWnd,
                    L"습관 삭제는 오늘로 이동하신 후 가능합니다.",
                    L"알림",
                    MB_OK
                );

                break;
            }

            wchar_t input[256];

            GetWindowTextW(
                hHabitInput,
                input,
                256
            );

            std::wstring wideHabitName(input);

            if (wideHabitName.empty())
            {
                EH::ShowEmptyInputError(hWnd);

                break;
            }

            int size = WideCharToMultiByte(
                CP_UTF8,
                0,
                wideHabitName.c_str(),
                -1,
                nullptr,
                0,
                nullptr,
                nullptr
            );

            if (size <= 0)
            {
                EH::ShowEmptyInputError(hWnd);

                break;
            }

            std::string habitName(
                size,
                '\0'
            );

            WideCharToMultiByte(
                CP_UTF8,
                0,
                wideHabitName.c_str(),
                -1,
                &habitName[0],
                size,
                nullptr,
                nullptr
            );

            habitName.pop_back();

            std::vector<Habit> habits =
                fileManager.LoadHabits();

            auto it = std::find_if(
                habits.begin(),
                habits.end(),
                [&](const Habit& habit)
                {
                    return habit.name == habitName;
                }
            );

            if (it == habits.end())
            {
                EH::ShowNoDataError(hWnd);

                break;
            }

            habits.erase(it);

            if (!fileManager.SaveHabits(habits))
            {
                EH::ShowSaveError(hWnd);

                break;
            }

            currentRecord.habits.erase(
                habitName
            );

            RecordManager recordManager;

            recordManager.AllCompleted(
                currentRecord
            );

            if (!fileManager.SaveRecord(currentRecord))
            {
                EH::ShowSaveError(hWnd);

                break;
            }

            if (currentRecord.date ==
                calendarManager.GetToday())
            {
                int straight =
                    recordManager.Count(
                        calendarManager.GetToday()
                    );

                std::wstring straightText =
                    L"연속 완료: " +
                    std::to_wstring(straight) +
                    L"일🔥🔥🔥";

                SetWindowTextW(
                    hSuccess,
                    straightText.c_str()
                );
            }

            CreateHabitList(hWnd);
            UpdateHabitList(hWnd);

            InvalidateRect(
                hSelectedDatePanel,
                nullptr,
                TRUE
            );

            UpdateWindow(
                hSelectedDatePanel
            );

            SetWindowTextW(
                hHabitInput,
                L""
            );

            MessageBoxW(
                hWnd,
                L"습관이 삭제되었습니다.",
                L"알림",
                MB_OK
            );

            break;
        }


        // 그 외 명령
        default:

            return DefWindowProc(
                hWnd,
                message,
                wParam,
                lParam
            );
        }

        break;
    }
	// 달력 선택 이벤트 처리
    case WM_NOTIFY:
    {
        LPNMHDR notification =
            reinterpret_cast<LPNMHDR>(lParam);

        if (notification->code == MCN_SELECT)
        {
            LPNMSELCHANGE selection =
                reinterpret_cast<LPNMSELCHANGE>(lParam);

            SYSTEMTIME selectedDate =
                selection->stSelStart;

            char dateBuffer[11];

            sprintf_s(
                dateBuffer,
                "%04d-%02d-%02d",
                selectedDate.wYear,
                selectedDate.wMonth,
                selectedDate.wDay
            );

            string date(dateBuffer);

            CalendarManager calendarManager;
            FileManager fileManager;
			RecordManager recordManager;

            std::string today = calendarManager.GetToday();

            if (date == today)
            {
                // 오늘 날짜
                if (fileManager.HasRecord(date))
                {
                    currentRecord =
                        fileManager.LoadRecord(date);
                }
                else
                {
                    std::vector<Habit> habits =
                        fileManager.LoadHabits();

                    currentRecord =
                        fileManager.CreateTodayRecord(
                            date,
                            habits
                        );
                }

            }
            else
            {
                // 과거 날짜
                currentRecord =
                    fileManager.LoadRecord(date);

                if (!fileManager.HasRecord(date))
                {
                    std::vector<Habit> habits =
                        fileManager.LoadHabits();

                    currentRecord.date = date;
                    currentRecord.allCompleted = false;

                    for (const auto& habit : habits)
                    {
                        if (habit.date <= date)
                        {
                            currentRecord.habits[habit.name] = false;
                        }
                    }
                }

            }
            // 화면 전체 갱신들
			CreateHabitList(hWnd);
            UpdateHabitList(hWnd);

            InvalidateRect(
                hSelectedDatePanel,
                nullptr,
                TRUE
            );

            int straight =
                recordManager.Count(today);

            std::wstring straightText =
                L"연속 완료: " +
                std::to_wstring(straight) +
                L"일🔥🔥🔥";

            SetWindowTextW(
                hSuccess,
                straightText.c_str()
            );

            bool isToday = (date == today);

            for (HWND hButton : hHabitButtons)
            {
                EnableWindow(
                    hButton,
                    isToday
                );
            }

            if (currentRecord.habits.empty())
            {
                EH::ShowNoDataError(hWnd);
            }
        }

        break;
    }

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            // TODO: 여기에 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
// 이하 코드들은 전부 UI 생성하는 함수들에 대한 윈도우 프로시저
LRESULT CALLBACK HabitListPanelProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rect;
        GetClientRect(hWnd, &rect);

        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        HPEN oldPen = (HPEN)SelectObject(hdc, hPen);

        Rectangle(
            hdc,
            1,
            1,
            rect.right,
            rect.bottom
        );

        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);

        DeleteObject(hBrush);
        DeleteObject(hPen);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        HWND hButton = (HWND)lParam;

        for (size_t i = 0; i < hHabitButtons.size(); i++)
        {
            if (hHabitButtons[i] == hButton)
            {
                string habitName = currentHabitNames[i];

                if (currentRecord.habits[habitName])
                {
                    SetBkColor(
                        hdc,
                        RGB(100, 200, 100)
                    );

                    return (LRESULT)hGreenBrush;
                }
                else
                {
                    SetBkColor(
                        hdc,
                        RGB(240, 100, 100)
                    );

                    return (LRESULT)hRedBrush;
                }
            }
        }

        break;
    }

    case WM_COMMAND:
    {
        HWND hParent = GetParent(hWnd);

        SendMessage(
            hParent,
            WM_COMMAND,
            wParam,
            lParam
        );

        return 0;
    }

    case WM_MOUSEWHEEL:
    {
        HWND hParent = GetParent(hWnd);

        SendMessage(
            hParent,
            WM_MOUSEWHEEL,
            wParam,
            lParam
        );

        return 0;
    }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK CalendarPanelProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {

    case WM_NOTIFY:
    {
        HWND hParent = GetParent(hWnd);

        SendMessage(
            hParent,
            WM_NOTIFY,
            wParam,
            lParam
        );

        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rect;
        GetClientRect(hWnd, &rect);

        HPEN hPen =
            CreatePen(
                PS_SOLID,
                2,
                RGB(0, 0, 0)
            );

        HPEN oldPen =
            (HPEN)SelectObject(hdc, hPen);

        Rectangle(
            hdc,
            1,
            1,
            rect.right,
            rect.bottom
        );

        SelectObject(hdc, oldPen);

        DeleteObject(hPen);

        EndPaint(hWnd, &ps);

        return 0;
    }
    }

    return DefWindowProc(
        hWnd,
        message,
        wParam,
        lParam
    );
}

LRESULT CALLBACK SuccessPanelProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rect;
        GetClientRect(hWnd, &rect);

        HPEN hPen =
            CreatePen(
                PS_SOLID,
                2,
                RGB(0, 0, 0)
            );

        HPEN oldPen =
            (HPEN)SelectObject(hdc, hPen);

        Rectangle(
            hdc,
            1,
            1,
            rect.right,
            rect.bottom
        );

        SelectObject(hdc, oldPen);

        DeleteObject(hPen);

        EndPaint(hWnd, &ps);

        return 0;
    }
    }

    return DefWindowProc(
        hWnd,
        message,
        wParam,
        lParam
    );
}

LRESULT CALLBACK TitlePanelProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rect;
        GetClientRect(hWnd, &rect);

        HBRUSH hBrush =
            CreateSolidBrush(RGB(255, 255, 255));

        HPEN hPen =
            CreatePen(
                PS_SOLID,
                2,
                RGB(0, 0, 0)
            );

        HBRUSH oldBrush =
            (HBRUSH)SelectObject(hdc, hBrush);

        HPEN oldPen =
            (HPEN)SelectObject(hdc, hPen);

        Rectangle(
            hdc,
            1,
            1,
            rect.right,
            rect.bottom
        );

        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);

        DeleteObject(hBrush);
        DeleteObject(hPen);

        EndPaint(hWnd, &ps);

        return 0;
    }
    }

    return DefWindowProc(
        hWnd,
        message,
        wParam,
        lParam
    );
}

LRESULT CALLBACK SelectedDatePanelProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rect;
        GetClientRect(hWnd, &rect);

        // 흰색 배경
        HBRUSH hBrush = CreateSolidBrush(
            RGB(255, 255, 255)
        );

        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);

        // 검은색 테두리
        HPEN hPen = CreatePen(
            PS_SOLID,
            2,
            RGB(0, 0, 0)
        );

        HPEN oldPen =
            (HPEN)SelectObject(hdc, hPen);

        HBRUSH oldBrush =
            (HBRUSH)SelectObject(
                hdc,
                GetStockObject(HOLLOW_BRUSH)
            );

        Rectangle(
            hdc,
            1,
            1,
            rect.right - 1,
            rect.bottom - 1
        );

        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);

        DeleteObject(hPen);

        // 날짜 출력
        SetBkMode(hdc, TRANSPARENT);

        COLORREF textColor;

        if (currentRecord.allCompleted)
        {
            textColor = RGB(0, 160, 0);
        }
        else
        {
            textColor = RGB(220, 0, 0);
        }

        SetTextColor(hdc, textColor);

        HFONT hFont = CreateFontW(
            22,
            0,
            0,
            0,
            FW_BOLD,
            FALSE,
            FALSE,
            FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS,
            L"맑은 고딕"
        );

        HFONT oldFont =
            (HFONT)SelectObject(hdc, hFont);

        DrawTextA(
            hdc,
            currentRecord.date.c_str(),
            -1,
            &rect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE
        );

        SelectObject(hdc, oldFont);
        DeleteObject(hFont);

        EndPaint(hWnd, &ps);
        return 0;
    }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
