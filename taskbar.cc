#include "stdshit.h"
#include "apihook.h"
#include <commctrl.h>
#include "tebpeb.h"
#include "explorer.h"
#define DrawText_MinLen 10
#include <conio.h>

BOOL drawPath(HDC hdc, LPCWSTR lpchText, LPRECT lpRect)
{
	// get path end
	LPCWSTR lastSlash = wcsrchr(lpchText, '\\');
	LPCWSTR pathEnd = lastSlash;
	for(int i = 0; lastSlash[i]; i++) {
		if(lastSlash[i] > ' ') pathEnd = &lastSlash[i+1];
		else if(lastSlash[i] == ' ') { //if(i >= DrawText_MinLen) break;
			if(*(DWORD*)(lastSlash+i+1) == 0x0020002D) break; }
	}

	// decide where to make the cuts
	SIZE size;
	GetTextExtentPoint32W(hdc, lpchText, pathEnd-lpchText, &size);
	if(size.cx <= (lpRect->right-lpRect->left))
		return DrawTextW(hdc, lpchText, -1, lpRect, 0x0824);
	GetTextExtentPoint32W(hdc, lastSlash, pathEnd-lastSlash, &size);
	if(size.cx > (lpRect->right-lpRect->left))
		return DrawTextW(hdc, lastSlash, -1, lpRect, 0x0824);
	return DrawTextW(hdc, lpchText, pathEnd-lpchText, lpRect,
		0x0824 | DT_RIGHT);
}

PVOID drawTextFunc; PVOID drawTextCall;
int WINAPI DrawText_hook1( HDC hDC, LPCTSTR lpchText,
 int nCount, LPRECT lpRect, UINT uFormat ) {
	drawTextCall = __builtin_return_address(0);
	return 0; }

int WINAPI DrawText_hook(HDC hDC, LPCWSTR lpchText,
	int nCount, LPRECT lpRect, UINT uFormat)
{
	if((drawTextCall == __builtin_return_address(0)) 
	&&(uFormat == 0x8824)) {
		for(int i = 0; (i < 10) && lpchText[i]; i++)
		if((lpchText[i] == ':')&&(lpchText[i+1] == '\\'))
			return drawPath(hDC, &lpchText[i-1], lpRect);
	}
	return DrawTextW(hDC, lpchText, nCount, lpRect, uFormat);
}

extern PVOID g_hinstCC;

extern "C"
DWORD WINAPI SHCreateDesktop(void* arg);
extern "C"
DWORD WINAPI SHCreateDesktop_hook(void* arg)
{
	//size_t base = (size_t)LoadLibraryA(
	//	"stobject.dll") - Stobject_Base;
	//MEMNOP(VolNotify_NopB+base, VolNotify_NopE+base);
	//CALLPATCH(VolNotify_Cp1+base, Volume_Timer+base);

	// register hook
	importDiscr id; id.fromBase(g_hinstCC);
	id.setFunc("user32.dll", "DrawTextW", 
		&DrawText_hook1, &drawTextFunc);

	// create dummy toolbar
	HWND hTool = CreateWindowExW(0, TOOLBARCLASSNAMEW, NULL, WS_VISIBLE, 
		0, 0, 0, 0,	0, (HMENU)0, GetModuleHandle(NULL), NULL);
	SendMessageW(hTool, TB_BUTTONSTRUCTSIZE,
		(WPARAM)sizeof(TBBUTTON), 0);
	TBBUTTON tbb = {0, 100, TBSTATE_ENABLED,
		TBSTYLE_BUTTON, {}, 0, (INT_PTR)TOOLBARCLASSNAMEW};
	SendMessageW(hTool, TB_ADDBUTTONSW, 1, (LPARAM)&tbb);
	SendMessageW(hTool, WM_PAINT, 0, 0);
	DestroyWindow(hTool);
	id.setFunc("user32.dll", 
		"DrawTextW", &DrawText_hook, 0);
		
	return SHCreateDesktop(arg);
}

extern "C"
int __thiscall CTaskBand___GetItemTitle(void *This, int iButton,
	LPWSTR pString, DWORD cchMaxCount, int a4);
extern "C"
int __thiscall CTaskBand___UpdateItemText(void *This, int iButton)
{
	WCHAR pString[MAX_PATH];
	TBBUTTONINFOW tbi; tbi.cbSize = sizeof(tbi);
	tbi.dwMask = 0x80000002; tbi.pszText = pString;
	CTaskBand___GetItemTitle(This, iButton, pString, MAX_PATH, 0);
	SendMessageW(((HWND*)This)[15], 0x440, iButton, (LPARAM)&tbi);
}
