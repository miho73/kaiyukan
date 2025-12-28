#include "framework.h"
#include "kaiyukan.h"

#define MAX_LOADSTRING 100

// 어딘가에 쓸 전역변수
HINSTANCE hInst;                                // 현재 인스턴스
WCHAR szTitle[MAX_LOADSTRING];                  // 윈도우 타이틀
WCHAR szWindowClass[MAX_LOADSTRING];            // 메인 윈도우 클래스 이름
ULONG_PTR gdiplusToken;                         // GDI+ 토큰

// 함수 선정의
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// 
// FUNCTION: wWinMain()
//
// PURPOSE: 애플리케이션의 진입점 및 주 메시지 루프를 처리
//
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR    lpCmdLine,
					  _In_ int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // 윈도우 정보 초기화
  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_KAIYUKAN, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // GDI+ 초기화
  GdiplusStartupInput gdiplusStartupInput;
  if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Ok)
	return FALSE;

  // 윈도우 초기화
  if (!InitInstance(hInstance, nCmdShow))
	return FALSE;

  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KAIYUKAN));

  MSG msg;

  // 메인 메시지 루프
  while (GetMessage(&msg, nullptr, 0, 0)) {
	if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
	  TranslateMessage(&msg);
	  DispatchMessage(&msg);
	}
  }

  // GDI+ 종료
  GdiplusShutdown(gdiplusToken);

  return (int)msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: 윈도우 클래스 등록
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;

  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KAIYUKAN));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = CreateSolidBrush(BG_COLOR);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_KAIYUKAN);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: 매인 윈도우 생성 및 표시
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  hInst = hInstance; // 인스턴스 핸들 전역변수로 보내기

  // 리소스 로드
  LoadOverlayImage();

  // 화면 크기 얻기
  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);

  DWORD dwExStyle = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW;

  // 경계없는 전체화면 윈도우 생성
  HWND hWnd = CreateWindowExW(
	dwExStyle,
	szWindowClass, szTitle, WS_POPUP,
	(screenWidth - JELLYFISH_SIZE) / 2, (screenHeight - JELLYFISH_SIZE) / 2,
	JELLYFISH_SIZE, JELLYFISH_SIZE,
	nullptr, nullptr, hInstance, nullptr
  );

  if (!hWnd)
	return FALSE;

  SetLayeredWindowAttributes(hWnd, BG_COLOR, 0, LWA_COLORKEY);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
	case WM_CREATE: {
	  SetTimer(hWnd, OVERLAY_TIMER_ID, 1000 / FPS, NULL); // 30 FPS 새로고침 주기
	  break;
	}

	case WM_COMMAND: {
	  int wmId = LOWORD(wParam);
	  handleMenuCommand(hWnd, wParam);
	  return DefWindowProc(hWnd, message, wParam, lParam);
	}

	case WM_PAINT: {
	  PAINTSTRUCT ps;
	  HDC hdc = BeginPaint(hWnd, &ps);

	  RECT clientRect;
	  GetClientRect(hWnd, &clientRect);

	  DrawOverlayContent(hdc, clientRect);

	  EndPaint(hWnd, &ps);
	  break;
	}

	case WM_TIMER: {
	  UpdateGifAnimation();
	  MotionWindowUpdate(hWnd);
	  InvalidateRect(hWnd, NULL, FALSE);
	  break;
	}

	case WM_LBUTTONDOWN: {
	  MotionWindowDown(hWnd);
	  break;
	}

	case WM_LBUTTONUP: {
	  MotionWindowUp();
	  break;
	}

	case WM_MOUSEMOVE: {
	  MotionWindowMove(hWnd);
	  break;
	}

	case WM_RBUTTONUP: {
	  showDialog(hWnd);
	  break;
	}

	case WM_DESTROY:
	KillTimer(hWnd, OVERLAY_TIMER_ID);
	PostQuitMessage(0);
	break;

	default:
	return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}
