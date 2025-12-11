#include "framework.h"
#include "kaiyukan.h"

// 타겟 윈도우 관련
const wchar_t* TARGET_WINDOW_NAME =L"kaiyukan - File Explorer";
int g_borderThickness = 4;

// 오버레이 이미지
Image* g_pGifImage = nullptr;
UINT g_frameCount = 0;
UINT g_currentFrame = 0;
Gdiplus::PropertyItem* g_pDelayProperty = nullptr;
ULONGLONG g_lastFrameChangeTime = 0;

void LoadOverlayImage() {
    g_pGifImage = new Image(L"assets/jellyfish.gif");

    if (g_pGifImage->GetLastStatus() == Gdiplus::Ok) {
        GUID pageGuid = FrameDimensionTime;
        g_frameCount = g_pGifImage->GetFrameCount(&pageGuid);

        int size = g_pGifImage->GetPropertyItemSize(PropertyTagFrameDelay);
        if (size > 0) {
            g_pDelayProperty = (Gdiplus::PropertyItem*)malloc(size);
            g_pGifImage->GetPropertyItem(PropertyTagFrameDelay, size, g_pDelayProperty);
        }

        g_currentFrame = 0;
        g_lastFrameChangeTime = GetTickCount64();
    }
}

void FollowTargetWindow(HWND hOverlayWnd) {
    HWND g_hTargetWnd = FindWindowW(NULL, TARGET_WINDOW_NAME);
	RECT g_targetRect;

    if (g_hTargetWnd) {
        GetWindowRect(g_hTargetWnd, &g_targetRect);

        SetWindowPos(
            hOverlayWnd, HWND_TOPMOST,
            g_targetRect.left, g_targetRect.top,
            JELLYFISH_SIZE, JELLYFISH_SIZE,
            SWP_NOACTIVATE | SWP_SHOWWINDOW
        );
    }
    else {
        ShowWindow(hOverlayWnd, SW_HIDE);
    }
}

void DrawOverlayContent(HDC hdc, const RECT& clientRect) {
	HDC hMemDC = CreateCompatibleDC(hdc);
    HBITMAP hMemBitmap = CreateCompatibleBitmap(hdc, JELLYFISH_SIZE, JELLYFISH_SIZE);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hMemBitmap);

    HBRUSH hBrushBg = CreateSolidBrush(BG_COLOR);
    FillRect(hMemDC, &clientRect, hBrushBg);
	DeleteObject(hBrushBg);	

    {
        Graphics graphics(hMemDC);

        if (
            g_pGifImage &&
            g_pGifImage->GetLastStatus() == Ok
        ) {
            graphics.DrawImage(
                g_pGifImage,
                0, 0, JELLYFISH_SIZE, JELLYFISH_SIZE
            );
        }
    }

    BitBlt(hdc, 0, 0, JELLYFISH_SIZE, JELLYFISH_SIZE, hMemDC, 0, 0, SRCCOPY);

    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hMemBitmap);
    DeleteDC(hMemDC);
}

void UpdateGifAnimation() {
    if (!g_pGifImage || g_frameCount <= 1 || !g_pDelayProperty) return;

    ULONGLONG currentTime = GetTickCount64();

    long frameDelay = ((long*)g_pDelayProperty->value)[g_currentFrame] * 10; // 1/100 -> ms

    if (frameDelay < 10) frameDelay = 100;

    if (currentTime - g_lastFrameChangeTime >= frameDelay) {
		g_currentFrame = (g_currentFrame + 1) % g_frameCount; // 다음 프레임으로 이동
        g_lastFrameChangeTime = currentTime; // 시간 리셋

		// 프레임 변경
        GUID pageGuid = Gdiplus::FrameDimensionTime;
        g_pGifImage->SelectActiveFrame(&pageGuid, g_currentFrame);
    }
}
