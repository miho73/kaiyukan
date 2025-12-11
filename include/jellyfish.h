#pragma once
#include <windows.h>
#define OVERLAY_TIMER_ID 2

void LoadOverlayImage();
void DrawOverlayContent(HDC hdc, const RECT& clientRect);
void FollowTargetWindow(HWND hOverlayWnd);
void UpdateGifAnimation();