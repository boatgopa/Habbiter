#pragma once

#include <windows.h>

class EH
{
public:
    static void ShowNoSelectionError(HWND hWnd);
    static void ShowSaveError(HWND hWnd);
    static void ShowDuplicateHabitError(HWND hWnd);
    static void ShowEmptyInputError(HWND hWnd);
    static void ShowNoDataError(HWND hWnd);
};