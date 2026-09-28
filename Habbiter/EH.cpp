#include "EH.h"

void EH::ShowNoSelectionError(HWND hWnd)
{
    MessageBoxW(
        hWnd,
        L"변경할 습관을 선택해주세요.",
        L"알림",
        MB_OK
    );
}

void EH::ShowSaveError(HWND hWnd)
{
    MessageBoxW(
        hWnd,
        L"데이터 저장에 실패했습니다.",
        L"오류",
        MB_OK
    );
}

void EH::ShowDuplicateHabitError(HWND hWnd)
{
    MessageBoxW(
        hWnd,
        L"이미 존재하는 습관입니다.",
        L"알림",
        MB_OK
    );
}

void EH::ShowEmptyInputError(HWND hWnd)
{
    MessageBoxW(
        hWnd,
        L"습관 이름을 입력해주세요.",
        L"알림",
        MB_OK
    );
}

void EH::ShowNoDataError(HWND hWnd)
{
    MessageBoxW(
        hWnd,
        L"이 날짜에는 기록이 없습니다.",
        L"알림",
        MB_OK
    );
}