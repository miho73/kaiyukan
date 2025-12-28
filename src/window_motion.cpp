#include "framework.h"
#include "kaiyukan.h"
#include <string>

bool isDragging = false;				// 드래그 중인지 여부
POINT initMousePos = { 0, 0 };			// 초기 마우스 위치
VECTOR velocity(0.0f, 0.0f);			// 속도 벡터
VECTOR heading(0.0f, -1.0f);				// 헤딩 벡터
float omega = 0.0f;						// 각속도
ULONGLONG dragStartTime = 0;			// 드래그 시작 시간
const float B_COEFF = 2.85f;			// 항력 계수
const float REFLECTION_COEFF = 0.75f;	// 반사 계수
const float INERTIA_MOMENT = 0.1f;		// 해파리의 tau - alpha 관계
const float TIME_STEP = 1.0f / FPS;		// 시간 간격
const float STOP_THRESHOLD = TIME_STEP;	// 정지 속도 임계값

// a = -bv
// -bvt = delta v

void MotionWindowDown(HWND hWnd) {
  if (isDragging) return;
  isDragging = true;

  SetCapture(hWnd);
  GetCursorPos(&initMousePos);
  dragStartTime = GetTickCount64();
}

void MotionWindowMove(HWND hWnd) {
  if (isDragging) {
	POINT currentMousePos;
	GetCursorPos(&currentMousePos);

	// 창 위치 확인
	RECT clientRect;
	GetWindowRect(hWnd, &clientRect);

	// 마우스가 창 밖으로 나가면 드래그 종료
	if (
	  currentMousePos.x > clientRect.right ||
	  currentMousePos.x < clientRect.left ||
	  currentMousePos.y > clientRect.bottom ||
	  currentMousePos.y < clientRect.top
	) {
	  isDragging = false;
	  ULONGLONG dt = GetTickCount64() - dragStartTime;
	  velocity.x = (currentMousePos.x - initMousePos.x) * 15 * pow(2.71828, -(long long)dt/800);
	  velocity.y = (currentMousePos.y - initMousePos.y) * 15 * pow(2.71828, -(long long)dt/800);
	  return;
	}
  }
}

void MotionWindowUp() {
  if (isDragging) {
	ReleaseCapture();

	POINT currentMousePos;
	GetCursorPos(&currentMousePos);

	isDragging = false;
	ULONGLONG dt = GetTickCount64() - dragStartTime;
	velocity.x = (currentMousePos.x - initMousePos.x) * 15 * pow(2.71828, -(long long)dt/800);
	velocity.y = (currentMousePos.y - initMousePos.y) * 15 * pow(2.71828, -(long long)dt/800);
  }
}

void MotionWindowUpdate(HWND hWnd) {
  if (isDragging) return;

  // 조기 종료
  if (fabs(velocity.x) < STOP_THRESHOLD && fabs(velocity.y) < STOP_THRESHOLD) return;

  RECT clientRect;
  GetWindowRect(hWnd, &clientRect);

  // 화면 크기 확인
  RECT screenRect;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);

  // 속도 벡터 업데이트
  velocity.x -= velocity.x * B_COEFF * TIME_STEP;
  velocity.y -= velocity.y * B_COEFF * TIME_STEP;

  // 미소 변위 벡터 계산
  VECTOR ds = velocity * TIME_STEP;

  // 경계 반사 처리
  if (ds.x + clientRect.left < 0) {
	velocity.x *= -REFLECTION_COEFF;
	ds.x = -ds.x - clientRect.left;
  }
  if (ds.y + clientRect.top < 0) {
	velocity.y *= -REFLECTION_COEFF;
	ds.y = -ds.y - clientRect.top;
  }
  if (ds.x + clientRect.right > screenRect.right) {
	velocity.x *= -REFLECTION_COEFF;
	ds.x = screenRect.right - clientRect.right - (clientRect.right + ds.x - screenRect.right);
  }
  if (ds.y + clientRect.bottom > screenRect.bottom) {
	velocity.y *= -REFLECTION_COEFF;
	ds.y = screenRect.bottom - clientRect.bottom - (clientRect.bottom + ds.y - screenRect.bottom);
  }

  // 순간 단위 속도벡터 계산
  VECTOR unit_velocity = velocity.normalize();
  heading = unit_velocity;
  /*
  float cosine = abs(unit_velocity.dot(heading));
  float torque = (velocity * sqrtf(1.0f - cosine * cosine)).magnitude();

  std::wstring log = L"LOC: " + std::to_wstring(cosine) + L", " + std::to_wstring(torque) + L"\n";
  OutputDebugStringW(log.c_str());

  // 각속도 업데이트
  omega += torque * INERTIA_MOMENT * TIME_STEP;

  // 헤딩 벡터 회전
  float s = sin(omega), c = cos(omega);
  heading.x = heading.x * c - heading.y * s;
  heading.y = heading.x * s + heading.y * c;
  heading = heading.normalize();
  */

  // 새 위치로
  SetWindowPos(
	hWnd, NULL,
	ds.x + clientRect.left, ds.y + clientRect.top,
	0, 0,
	SWP_NOSIZE | SWP_NOZORDER
  );

  // 속도가 정지 조건보다 작아지면 정지
  if (velocity.x * velocity.x + velocity.y * velocity.y < STOP_THRESHOLD * STOP_THRESHOLD) {
	velocity.x = 0.0f;
	velocity.y = 0.0f;
	return;
  }
}
