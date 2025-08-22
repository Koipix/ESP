#include <windows.h>

int posX, posY = 100;
int boxW = 100, boxH = 500;

void DrawESP(HDC hdc) {
  int left = posX;
  int top = posY;
  int right = posX + boxW;
  int bottom = posY + boxH;
  HPEN pen = CreatePen(PS_SOLID, 3, RGB(255,0,0));
  HBRUSH hollow = (HBRUSH) GetStockObject(NULL_BRUSH);
  SelectObject(hdc, pen);
  SelectObject(hdc, hollow);
  Rectangle(hdc, left, top, right, bottom);
  DeleteObject(pen);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
      case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        HBRUSH bg = CreateSolidBrush(RGB(0,0,0));
        FillRect(hdc, &rc, bg);
        DeleteObject(bg);

        DrawESP(hdc);
        EndPaint(hwnd, &ps);
      } break;

      case WM_TIMER: {
        RECT rc;
        GetClientRect(hwnd, &rc);
        
        posX += 5;
        InvalidateRect(hwnd, NULL, FALSE);
      } break;

      case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

      default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreviewInstance, LPSTR lpCmdLine, int nCmdShow) {
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);
  int screenWidth = GetSystemMetrics(SM_CXSCREEN);

  const char CLASS_NAME[] = "TransparentOverlay";

  WNDCLASS wc = {0};
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;
  wc.hbrBackground = NULL;

  RegisterClass(&wc);

  HWND hwnd = CreateWindowEx(
    WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
    CLASS_NAME, "Box",
    WS_POPUP,
    0, 0, screenWidth, screenHeight,
    NULL, NULL, hInstance, NULL
  );

  SetLayeredWindowAttributes(hwnd, RGB(0,0,0), 0, LWA_COLORKEY);

  if (hwnd == NULL) return 0;

  ShowWindow(hwnd, nCmdShow);
  SetTimer(hwnd, 1, 16, NULL); //16ms = 60fps

  MSG msg = {0};
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return 0;
}


