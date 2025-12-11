#include "framework.h"
#include "kaiyukan.h"
#include <string>

bool g_isDragging = false;				// 드래그 중인지 여부
POINT g_lastMousePos = { 0, 0 };		// 마지막 마우스 위치
float g_velocityX = 0.0f;				// X축 속도
float g_velocityY = 0.0f;				// Y축 속도
const float B_COEFF = 2.85f;			// 항력 계수
const float REFLECTION_COEFF = 0.75f;	// 반사 계수
const float STOP_THRESHOLD = 0.6f;		// 정지 속도 임계값
const float TIME_STEP = 1.0f / FPS;		// 시간 간격

// a = -bv
// -bvt = delta v

void MotionWindowDown(HWND hWnd) {
	g_isDragging = true;

	SetCapture(hWnd);
	GetCursorPos(&g_lastMousePos);

	g_velocityX = 0.0f;
	g_velocityY = 0.0f;
}

void MotionWindowMove(HWND hWnd) {
	if (g_isDragging) {
		POINT currentMousePos;
		GetCursorPos(&currentMousePos);

		// 창 위치 확인
		RECT clientRect;
		GetWindowRect(hWnd, &clientRect);

		// 화면 크기 확인
		RECT screenRect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);

		int dx = currentMousePos.x - g_lastMousePos.x;
		int dy = currentMousePos.y - g_lastMousePos.y;

		if (dx + clientRect.left < 0) dx = 0;
		if (dy + clientRect.top < 0) dy = 0;
		if (dx + clientRect.right > screenRect.right) dx = screenRect.right - clientRect.right;
		if (dy + clientRect.bottom > screenRect.bottom) dy = screenRect.bottom - clientRect.bottom;

		int nx = dx + clientRect.left;
		int ny = dy + clientRect.top;

		// 창 위치 업데이트
		SetWindowPos(
			hWnd, NULL,
			nx, ny,
			0, 0,
			SWP_NOSIZE | SWP_NOZORDER
		);

		// 속도 업데이트 (v=x/t)
		g_velocityX = dx / TIME_STEP;
		g_velocityY = dy / TIME_STEP;

		// 최종 마우스 위치 업데이트
		g_lastMousePos = currentMousePos;
	}
}

void MotionWindowUp() {
	if (g_isDragging) {
		ReleaseCapture();
		g_isDragging = false;
	}
}

void MotionWindowUpdate(HWND hWnd) {
	std::wstring log = L"LOC: " + std::to_wstring(g_velocityX) + L", " + std::to_wstring(g_velocityY) + L"\n";
	OutputDebugStringW(log.c_str());

	if (g_isDragging) return;

	// 조기 종료
	if (fabs(g_velocityX) < STOP_THRESHOLD && fabs(g_velocityY) < STOP_THRESHOLD) return;

	RECT clientRect;
	GetWindowRect(hWnd, &clientRect);

	// 화면 크기 확인
	RECT screenRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);

	int dx = g_velocityX * TIME_STEP;
	int dy = g_velocityY * TIME_STEP;

	// 경계 반사 처리
	if (dx + clientRect.left < 0) {
		g_velocityX *= -REFLECTION_COEFF;
		dx *= -1;
	}
	if (dy + clientRect.top < 0) {
		g_velocityY *= -REFLECTION_COEFF;
		dy *= -1;
	}
	if (dx + clientRect.right > screenRect.right) {
		g_velocityX *= -REFLECTION_COEFF;
		dx = screenRect.right - clientRect.right - (clientRect.right + dx - screenRect.right);
	}
	if (dy + clientRect.bottom > screenRect.bottom) {
		g_velocityY *= -REFLECTION_COEFF;
		
	}

	// 새 위치로
	SetWindowPos(
		hWnd, NULL,
		dx + clientRect.left, dy + clientRect.top,
		0, 0,
		SWP_NOSIZE | SWP_NOZORDER
	);

	g_velocityX -= g_velocityX * B_COEFF * TIME_STEP;
	g_velocityY -= g_velocityY * B_COEFF * TIME_STEP;

	// 속도가 정지 조건보다 작아지면 정지
	if (g_velocityX * g_velocityX + g_velocityY * g_velocityY < STOP_THRESHOLD * STOP_THRESHOLD) {
		g_velocityX = 0.0f;
		g_velocityY = 0.0f;
		return;
	}
}
