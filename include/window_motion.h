#pragma once
#include <windows.h>
#include "vector.h"

extern VECTOR heading;

void MotionWindowDown(HWND hWnd);
void MotionWindowUp();
void MotionWindowMove(HWND hWnd);
void MotionWindowUpdate(HWND hWnd);