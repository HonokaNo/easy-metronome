#include <windows.h>
#include <stdio.h>

static int bpm;

LRESULT CALLBACK WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam){
  static BOOL play = FALSE;
  PAINTSTRUCT ps;
  HDC hdc;
  static HBRUSH s_hbrRed = NULL, s_hbrBlue = NULL;
  HGDIOBJ hbrOld;

  switch(Msg){
    case WM_CREATE:
      s_hbrBlue = CreateSolidBrush(RGB(0x00, 0x00, 0xff));
      s_hbrRed = CreateSolidBrush(RGB(0xff, 0x00, 0x00));
      mciSendString("Open click.mp3 alias click", "", 0, 0);
      SetTimer(hWnd, 1, 1000 * 60 / bpm, NULL);
      return 0;
    case WM_DESTROY:
      KillTimer(hWnd, 1);
      mciSendString("Stop click", "", 0, 0);
      mciSendString("Close All", "", 0, 0);
      DeleteObject(s_hbrBlue);
      DeleteObject(s_hbrRed);
      PostQuitMessage(0);
      return 0;
    case WM_LBUTTONDOWN:
      play = !play;
      InvalidateRect(hWnd, NULL, FALSE);
      return 0;
    case WM_TIMER:
      if(play){
        mciSendString("Play click from 0", "", 0, 0);
      }
      return 0;
    case WM_PAINT:
    {
      RECT rc;
      GetClientRect(hWnd, &rc);
      hdc = BeginPaint(hWnd, &ps);
      if(play){
        hbrOld = SelectObject(hdc, s_hbrBlue);
        Rectangle(hdc, 0, 0, rc.right, rc.bottom);
        DrawTextA(hdc, "Playing...", -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      }else{
        hbrOld = SelectObject(hdc, s_hbrRed);
        Rectangle(hdc, 0, 0, rc.right, rc.bottom);
        DrawTextA(hdc, "Please click me to start...", -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      }
      SelectObject(hdc, hbrOld);
      EndPaint(hWnd, &ps);
      return 0;
    }
  }

  return DefWindowProc(hWnd, Msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
  HWND hWnd;
  WNDCLASS wndClass;
  static TCHAR strText[MAX_PATH], strPath[MAX_PATH];
  DWORD style;

  GetCurrentDirectory(MAX_PATH, strText);
  wsprintf(strPath, TEXT("%s\\%s"), strText, TEXT("settings.ini"));

  bpm = GetPrivateProfileInt(TEXT("General"), TEXT("BPM"), 120, strPath);
  if (bpm <= 0)
    bpm = 120;

  wndClass.style = CS_HREDRAW | CS_VREDRAW;
  wndClass.lpfnWndProc = WinProc;
  wndClass.cbClsExtra = wndClass.cbWndExtra = 0;
  wndClass.hInstance = hInstance;
  wndClass.hIcon = LoadIcon(NULL, IDI_INFORMATION);
  wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndClass.hbrBackground = CreateSolidBrush(RGB(0xff, 0x00, 0x00));
  wndClass.lpszMenuName = NULL;
  wndClass.lpszClassName = TEXT("METRONOME");

  if(!RegisterClass(&wndClass)) return -1;

  style = (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
  hWnd = CreateWindow(TEXT("METRONOME"), TEXT("Metronome"), style, CW_USEDEFAULT, CW_USEDEFAULT, 300, 100, NULL, NULL, hInstance, NULL);
  if(!hWnd) return -1;

  UpdateWindow(hWnd);

  MSG msg;
  while(GetMessage(&msg, NULL, 0, 0)){
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  DeleteObject(wndClass.hbrBackground);

  return (INT)msg.wParam;
}
