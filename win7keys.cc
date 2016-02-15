#include <windows.h>
#include <string.h>
#include <conio.h>
#include "clipswp.cc"

extern HWND v_hwndDesktop;
extern HWND v_hwndTray;
enum {
	GHID_SNAPTOPP = 512,
	GHID_SNAPBOTT = 513,
	GHID_SNAPLEFT = 514,
	GHID_SNAPRGHT = 515,
	GHID_TOPMOST = 516,
	GHID_CLIPSWP = 517
};
	
static
int GetWindowBorders(LONG lStyle, DWORD dwExStyle) 
{
	int cBorders = 0;
	if (dwExStyle & WS_EX_WINDOWEDGE)
		cBorders += 2;
	else if (dwExStyle & WS_EX_STATICEDGE)
		++cBorders;
	if ( (lStyle & WS_CAPTION) || (dwExStyle & WS_EX_DLGMODALFRAME) )
		++cBorders;	
	if (lStyle & WS_SIZEBOX) { DWORD gclBorder;
		SystemParametersInfo(SPI_GETBORDER, 0, &gclBorder, 0);
		cBorders += gclBorder; }
	return cBorders;
}

static
DWORD getMaximizeRect(HWND hwnd, RECT* rect)
{
	MONITORINFO monInfo; monInfo.cbSize = sizeof(monInfo);
	GetMonitorInfo(MonitorFromWindow(hwnd,
		MONITOR_DEFAULTTONEAREST), &monInfo);
	LONG lStyle = GetWindowLong(hwnd, GWL_STYLE);
	int cBorders = GetWindowBorders(lStyle, GetWindowLong(hwnd, GWL_EXSTYLE));
	rect->left = monInfo.rcWork.left-cBorders;
	rect->right = monInfo.rcWork.right+cBorders;
	rect->top = monInfo.rcWork.top-cBorders;
	rect->bottom = monInfo.rcWork.bottom+cBorders;	
	return lStyle;
}

void WINAPI HandleWin7Hotkey(WPARAM wParam)
{
	// get forground window
	HWND hwnd = GetForegroundWindow();
	if((hwnd == NULL) || (hwnd == v_hwndDesktop)
	|| (hwnd == v_hwndTray)) return;
	if(wParam == GHID_TOPMOST) {
		BOOL topMost = GetWindowLong(hwnd,
			GWL_EXSTYLE) & WS_EX_TOPMOST;
		SetWindowPos(hwnd, topMost ? HWND_NOTOPMOST :
			HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		return;
	}
	
	// get resize mode
	int modex = 0; int modey = 0;
	switch(wParam) {
	if(0) { case GHID_SNAPTOPP: modey = -1; }
	if(0) { case GHID_SNAPBOTT: modey = 1; }
		if(GetKeyState(VK_LEFT) < 0) modex = -1;
		else if(GetKeyState(VK_RIGHT) < 0) modex = 1;
		break;
	if(0) { case GHID_SNAPLEFT: modex = -1; }
	if(0) { case GHID_SNAPRGHT: modex = 1; }
		if(GetKeyState(VK_UP) < 0) modey = -1;
		else if(GetKeyState(VK_DOWN) < 0) modey = 1;
		break;
	}
	
	// determin new dimentions
	RECT maxiRect; 
	DWORD style = getMaximizeRect(hwnd, &maxiRect);	
	int width, height;
	if(!(style & (WS_SIZEBOX | WS_SIZEBOX))) {
		RECT rect; GetWindowRect(hwnd, &rect);
		width = rect.right-rect.left; height = rect.bottom-rect.top;
	} else {
		MINMAXINFO mmi = {0}; 
		mmi.ptMaxSize.x = maxiRect.right-maxiRect.left;			
		if(modex) mmi.ptMaxSize.x /= 2;
		mmi.ptMaxSize.y = maxiRect.bottom-maxiRect.top;
		if(modey) mmi.ptMaxSize.y /= 2;
		SendMessage(hwnd, WM_GETMINMAXINFO, 0, (LPARAM)&mmi);
		width = mmi.ptMaxSize.x; height = mmi.ptMaxSize.y;
		ShowWindow(hwnd, SW_RESTORE);
	}
	
	// resize window
	int topp = (modey > 0) ? maxiRect.bottom-height : maxiRect.top;
	int left = (modex > 0) ? maxiRect.right-width : maxiRect.left;
	MoveWindow(hwnd, left, topp, width, height, TRUE);
}

extern "C"
void __thiscall HandleGlobalHotkey(void* This, WPARAM wParam);
extern "C"
void __thiscall HandleGlobalHotkey_hook(void* This, WPARAM wParam)
{
	if((wParam >= GHID_SNAPTOPP)&&(wParam <= GHID_TOPMOST))
		HandleWin7Hotkey(wParam);
	if(wParam == GHID_CLIPSWP)
		HandleClipHotkey(This, wParam);
	return HandleGlobalHotkey(This, wParam);
}

extern const DWORD GlobalKeylist[];
const DWORD GlobalKeylist[] =
{	  MAKELONG(TEXT('R'), MOD_WIN),					// 500
      MAKELONG(TEXT('M'), MOD_WIN),					// 501
      MAKELONG(TEXT('M'), MOD_SHIFT|MOD_WIN),		// 502
      MAKELONG(VK_F1,MOD_WIN),						// 503
      MAKELONG(TEXT('E'),MOD_WIN),					// 504
      MAKELONG(TEXT('F'),MOD_WIN),					// 505
      MAKELONG(TEXT('F'), MOD_CONTROL|MOD_WIN),		// 506
      MAKELONG(VK_TAB, MOD_WIN),					// 507
      MAKELONG(VK_TAB, MOD_WIN|MOD_SHIFT),			// 508
      MAKELONG(VK_PAUSE,MOD_WIN),					// 509
      MAKELONG(TEXT('D'),MOD_WIN),					// 510
	  MAKELONG(TEXT('B'),MOD_WIN),					// 511
	  MAKELONG(VK_UP,MOD_WIN),						// 512
	  MAKELONG(VK_DOWN,MOD_WIN),					// 513
	  MAKELONG(VK_LEFT,MOD_WIN),  					// 514
	  MAKELONG(VK_RIGHT,MOD_WIN),					// 515
	  MAKELONG(VK_SPACE,MOD_WIN),					// 516
	  MAKELONG(TEXT('V'),MOD_WIN)					// 517
};
