#include "dialog.h"

void showDialog(HWND hWnd) {
  HMENU hMenu = CreatePopupMenu();

  //AppendMenu(hMenu, MF_STRING, ID_MENU_FEED, L"먹이 주기");
  //AppendMenu(hMenu, MF_SEPARATOR, 0, NULL); // 가로 줄(구분선)
  AppendMenu(hMenu, MF_STRING, ID_MENU_EXIT, L"잘가 젤리피시");

  POINT pt;
  GetCursorPos(&pt);

  TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
  DestroyMenu(hMenu);
}

void handleMenuCommand(HWND hWnd, WPARAM wParam) {
  switch (LOWORD(wParam)) {
	case ID_MENU_FEED: {

	}
	case ID_MENU_EXIT: {
	  PostMessage(hWnd, WM_CLOSE, 0, 0);
	  break;
	}
  }
}