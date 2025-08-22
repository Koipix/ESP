#include <windows.h>

RECT box = {50, 50, 150, 150};
int spd = 10;

void DrawESP(HDC hdc) {
  int left = box.left;
  int top = box.top;
  int right = box.right;
  int bottom = box.bottom;

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
        
        RECT rc;
        GetClientRect(hwnd, &rc);
        
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        HDC hdc = BeginPaint(hwnd, &ps);
        HDC memDC = CreateCompatibleDC(hdc);

        HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);

        HBRUSH bg = CreateSolidBrush(RGB(0,0,0));
        
        FillRect(memDC, &rc, bg);
        DrawESP(memDC);

        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
        
        SelectObject(memDC, oldBitmap);
        DeleteObject(bitmap);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        return 0;
      } break;
      
      //debugging..
      case WM_KEYDOWN:
        switch (wParam) {
          case VK_LEFT: OffsetRect(&box, -spd, 0); break;
          case VK_RIGHT: OffsetRect(&box, spd, 0); break;
          case VK_UP: OffsetRect(&box, 0, -spd); break;
          case VK_DOWN: OffsetRect(&box, 0, spd); break;
      }

      case WM_TIMER: {
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


