#include <windows.h>
#include <windowsx.h>

const unsigned int MAIN_TIMER = 1;
const unsigned int WND_WIDTH = 1000;
const unsigned int WND_HEIGHT = 600;
const int RECT_WIDTH = 50;
const int RECT_HEIGHT = 50;
const int FPS = 60;

int WIDTH;
int HEIGHT;
char szClassName[] = "LABA 1";
float speedX = 0.0;
float speedY = 0.0;
float acceleration = 1.5;
float decceleration = 1.0;
float collisionSpeedSave = 0.7;
float wheelSpin = 0;

RECT gRectangle = { 300, 300, 300 + RECT_WIDTH, 300 + RECT_HEIGHT };
HBITMAP hBitmap = NULL;
HBITMAP hMaskBitmap = NULL;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void MoveRectangle(RECT* rect, float collisionSpeedSave);
void Clear();
void DrawSprite(HDC hdc);
BOOL Main_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void Main_OnDestroy(HWND hWnd);
void Main_OnTimer(HWND hWnd);
void Main_OnPaint(HWND hWnd);
void Main_OnMouseWheel(HWND hwnd, int zDelta);
void Main_OnSize(HWND hwnd, UINT state, int cx, int cy);
void Main_OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HWND hWnd;
    MSG lpMsg;
    WNDCLASS wc;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = (LPCWSTR)szClassName;
    
    RegisterClass(&wc);

    hWnd = CreateWindow(
        (LPCWSTR)szClassName,
        L"WinAPI",
        WS_OVERLAPPEDWINDOW,
        50, 50,
        WND_WIDTH, WND_HEIGHT,
        (HWND)NULL,
        (HMENU)NULL,
        (HINSTANCE)hInstance,
        NULL);

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    WIDTH = clientRect.right - clientRect.left;
    HEIGHT = clientRect.bottom - clientRect.top;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&lpMsg, NULL, 0, 0))
    {
        TranslateMessage(&lpMsg);
        DispatchMessage(&lpMsg);
    }
    return (lpMsg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
    switch (messg)
    {
    case WM_PAINT:
        Main_OnPaint(hWnd);
        break;

    case WM_CREATE:
        Main_OnCreate(hWnd, (LPCREATESTRUCT)lParam);
        break;

    case WM_MOUSEWHEEL:
        Main_OnMouseWheel(hWnd, (int)GET_WHEEL_DELTA_WPARAM(wParam));
        break;

    case WM_TIMER:
        Main_OnTimer(hWnd);
        break;

    case WM_DESTROY:
        Main_OnDestroy(hWnd);
        break;

    case WM_SIZE:
        Main_OnSize(hWnd, (UINT)wParam, LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_GETMINMAXINFO:
        Main_OnGetMinMaxInfo(hWnd, (LPMINMAXINFO)lParam);
        break;

    default:
        return DefWindowProc(hWnd, messg, wParam, lParam);
    }
    return 0;
}

BOOL Main_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    SetTimer(hWnd, MAIN_TIMER, 1000 / FPS, NULL);
    hBitmap = (HBITMAP)LoadImage(NULL, L"D:\\spritbmp.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
    hMaskBitmap = (HBITMAP)LoadImage(NULL, L"D:\\spritmask.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
    return TRUE;
}

void Main_OnDestroy(HWND hWnd)
{
    PostQuitMessage(0);
    KillTimer(hWnd, MAIN_TIMER);
    Clear();
}

void Main_OnTimer(HWND hWnd)
{
    bool isUpPressed = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
    bool isDownPressed = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
    bool isLeftPressed = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
    bool isRightPressed = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
    bool isShiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    if (isDownPressed) {
        speedY += acceleration;
    }
    else if (isUpPressed) {
        speedY -= acceleration;
    }
    else {
        if (speedY > -decceleration && speedY < decceleration) {
            speedY = 0;
        }
        else if (speedY > 0) {
            speedY -= decceleration;
        }
        else if (speedY < 0) {
            speedY += decceleration;
        }
    }
    if (isRightPressed) {
        speedX += acceleration;
    }
    else if (isLeftPressed) {
        speedX -= acceleration;
    }
    else {
        if (speedX > -decceleration && speedX < decceleration) {
            speedX = 0;
        }
        else if (speedX > 0) {
            speedX -= decceleration;
        }
        else if (speedX < 0) {
            speedX += decceleration;
        }
    }

    if (isShiftPressed && wheelSpin != 0) {
        speedX += wheelSpin;
    }
    else {
        speedY -= wheelSpin;
    }
    wheelSpin = 0;

    MoveRectangle(&gRectangle, collisionSpeedSave);

    InvalidateRect(hWnd, NULL, FALSE);
}

void Main_OnMouseWheel(HWND hwnd, int zDelta)
{
    wheelSpin += zDelta / 15;
}

void Main_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    if (clientRect.right - clientRect.left != 0) {
        WIDTH = clientRect.right - clientRect.left;
    }
    if (clientRect.bottom - clientRect.top != 0) {
        HEIGHT = clientRect.bottom - clientRect.top;
    }
}

void Main_OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo)
{
    lpMinMaxInfo->ptMinTrackSize.x = 300;
    lpMinMaxInfo->ptMinTrackSize.y = 300;
}

void MoveRectangle(RECT* rect, float collisionSpeedSave) {
    if (rect->left + speedX < 0) {
        rect->left = 0;
        rect->right = RECT_WIDTH;
        speedX = -speedX * collisionSpeedSave;
    }
    else if (rect->right + speedX > WIDTH) {
        rect->left = WIDTH - RECT_WIDTH;
        rect->right = WIDTH;
        speedX = -speedX * collisionSpeedSave;
    }
    else {
        rect->left += int(speedX);
        rect->right += int(speedX);
    }

    if (rect->top + speedY < 0) {
        rect->top = 0;
        rect->bottom = RECT_HEIGHT;
        speedY = -speedY * collisionSpeedSave;
    }
    else if (rect->bottom + speedY > HEIGHT) {
        rect->top = HEIGHT - RECT_HEIGHT;
        rect->bottom = HEIGHT;
        speedY = -speedY * collisionSpeedSave;
    }
    else {
        rect->top += int(speedY);
        rect->bottom += int(speedY);
    }
}

void DrawSprite(HDC hdc) {
    HDC hdcMask = CreateCompatibleDC(hdc);
    HDC hdcSource = CreateCompatibleDC(hdc);
    HDC hdcInvertedMask = CreateCompatibleDC(hdc);
    HBITMAP hBlankBitmap = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);

    SelectObject(hdcMask, hMaskBitmap);
    SelectObject(hdcSource, hBitmap);
    SelectObject(hdcInvertedMask, hBlankBitmap);

    BitBlt(hdcInvertedMask, 0, 0, WIDTH, HEIGHT, hdcMask, 0, 0, NOTSRCCOPY);
    BitBlt(hdcSource, 0, 0, WIDTH, HEIGHT, hdcInvertedMask, 0, 0, SRCAND);
    BitBlt(hdc, gRectangle.left, gRectangle.top, RECT_WIDTH, RECT_HEIGHT, hdcMask, 0, 0, SRCAND);
    BitBlt(hdc, gRectangle.left, gRectangle.top, RECT_WIDTH, RECT_HEIGHT, hdcSource, 0, 0, SRCPAINT);

    DeleteDC(hdcMask);
    DeleteDC(hdcSource);
    DeleteDC(hdcInvertedMask);
    DeleteObject(hBlankBitmap);
}

void Main_OnPaint(HWND hWnd)
{
    HDC hdc, hCmpDC;
    HBITMAP hBmp;
    RECT Rect;
    PAINTSTRUCT ps;

    GetClientRect(hWnd, &Rect);
    hdc = BeginPaint(hWnd, &ps);

    hCmpDC = CreateCompatibleDC(hdc);
    hBmp = CreateCompatibleBitmap(hdc, Rect.right - Rect.left, Rect.bottom - Rect.top);
    SelectObject(hCmpDC, hBmp);
    LOGBRUSH br;
    br.lbStyle = BS_SOLID;
    br.lbColor = RGB(0, 255, 0);
    HBRUSH brush;
    brush = CreateBrushIndirect(&br);
    FillRect(hCmpDC, &Rect, brush);
    DeleteObject(brush);

    DrawSprite(hCmpDC);

    BitBlt(hdc, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top,
        hCmpDC, 0, 0, SRCCOPY);

    DeleteDC(hCmpDC);
    DeleteObject(hBmp);
    hCmpDC = NULL;

    EndPaint(hWnd, &ps);
}

void Clear() {
    DeleteObject(hBitmap);
    DeleteObject(hMaskBitmap);
}
