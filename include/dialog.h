#pragma once

#include "framework.h"
#define ID_MENU_FEED  1001
#define ID_MENU_EXIT  1002

void showDialog(HWND hWnd);
void handleMenuCommand(HWND hWnd, WPARAM wParam);
