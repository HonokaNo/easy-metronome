#include <windows.h>
#include <stdio.h>

static int bpm;

LRESULT CALLBACK WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam){
  static BOOL play = FALSE;
  PAINTSTRUCT ps;
  HDC hdc;

  switch(Msg){
    case WM_CREATE:
      mciSendString("Open click.mp3 alias click", "", 0, 0);
      return 0;
    case WM_DESTROY:
      mciSendString("Stop click", "", 0, 0);
      mciSendString("Close All", "", 0, 0);
      PostQuitMessage(0);
      return 0;
    case WM_LBUTTONDOWN:
      play = !play;
      InvalidateRect(hWnd, NULL, FALSE);
      return 0;
    case WM_TIMER:
      if(play){
        mciSendString("Play click from 0", "", 0, 0);
        SetTimer(hWnd, 1, 1000 * 60 / bpm, NULL);
      }
      return 0;
    case WM_PAINT:
      hdc = BeginPaint(hWnd, &ps);
      if(play){
        SelectObject(hdc , CreateSolidBrush(RGB(0x00, 0x00, 0xff)));
      }else{
        SelectObject(hdc , CreateSolidBrush(RGB(0xff, 0x00, 0x00)));
      }
      Rectangle(hdc , 0 , 0 , 300 , 100);
      DeleteObject(SelectObject(hdc, GetStockObject(WHITE_BRUSH)));
      EndPaint(hWnd, &ps);
      return 0;
  }

  return DefWindowProc(hWnd, Msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
  HWND hWnd;
  WNDCLASS wndClass;
  static TCHAR strText[MAX_PATH], strPath[MAX_PATH];

  GetCurrentDirectory(MAX_PATH, strText);
  wsprintf(strPath, TEXT("%s\\%s"), strText, TEXT("settings.ini"));

  bpm = GetPrivateProfileInt(TEXT("General"), TEXT("BPM"), 120, strPath);

  wndClass.style = CS_HREDRAW | CS_VREDRAW;
  wndClass.lpfnWndProc = WinProc;
  wndClass.cbClsExtra = wndClass.cbWndExtra = 0;
  wndClass.hInstance = hInstance;
  wndClass.hIcon = NULL;
  wndClass.hCursor = NULL;
  wndClass.hbrBackground = CreateSolidBrush(RGB(0xff, 0x00, 0x00));
  wndClass.lpszMenuName = NULL;
  wndClass.lpszClassName = TEXT("METRONOME");

  if(!RegisterClass(&wndClass)) return -1;

  hWnd = CreateWindow(TEXT("METRONOME"), TEXT("Metronome"), (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 100, NULL, NULL, hInstance, NULL);
  if(!hWnd) return -1;

  SetTimer(hWnd, 1, 1000 * 60 / bpm, NULL);

  MSG msg;
  while(GetMessage(&msg, NULL, 0, 0)){
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return msg.wParam;
}
