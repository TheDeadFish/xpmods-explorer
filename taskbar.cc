#include "stdshit.h"
#include "apihook.h"
#include <commctrl.h>
#include "tebpeb.h"
#include "explorer.h"
#define DrawText_MinLen 10
#include <conio.h>
#include <shlwapi.h>

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

PVOID drawTextCall;
int WINAPI DrawText_hook(HDC hDC, LPCWSTR lpchText,
	int nCount, LPRECT lpRect, UINT uFormat)
{
	if((drawTextCall == __builtin_return_address(0)) 
	&&(!(~uFormat & 0x8824))) {
		for(int i = 0; (i < 10) && lpchText[i]; i++)
		if((lpchText[i] == ':')&&(lpchText[i+1] == '\\'))
			return drawPath(hDC, &lpchText[i-1], lpRect);
	}
	return DrawTextW(hDC, lpchText, nCount, lpRect, uFormat);
}

extern PVOID g_hinstCC;
int WINAPI DrawText_hook1( HDC hDC, LPCTSTR lpchText,
 int nCount, LPRECT lpRect, UINT uFormat ) {
	if(!(~uFormat & 0x8824)) {
		drawTextCall = __builtin_return_address(0);
		importDiscr id; id.fromBase(g_hinstCC);
		id.setFunc("user32.dll", "DrawTextW",
			&DrawText_hook, 0);
	}
	return DrawTextW(hDC, lpchText, nCount, lpRect, uFormat);
}

extern "C" BOOL WINAPI SHCreateThread_hook(
	LPTHREAD_START_ROUTINE a, void* b, 
	DWORD c, LPTHREAD_START_ROUTINE d)
{
	importDiscr id; id.fromBase(g_hinstCC);
	id.setFunc("user32.dll", "DrawTextW",
		&DrawText_hook1, 0);

	return SHCreateThread(a, b, c, d);
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
	SendMessageW(CTASKBAND_HTOOLBAR(This), 
		0x440, iButton, (LPARAM)&tbi);
}
