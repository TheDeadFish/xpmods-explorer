#include <stdio.h>
#include <string.h>
#include "callpatch.h"
#include <windows.h>

class UnProtect
{
public:
	UnProtect(void* base, int length)
	{
		size_t funcBase = (size_t)base;
		pageBase = funcBase & ~4095;
		pageEnd = (funcBase+4095+length) & ~4095;
		VirtualProtect((PVOID)pageBase, pageEnd-pageBase,
			PAGE_EXECUTE_READWRITE, &flOldProtect);	
	}
	~UnProtect()
	{
		VirtualProtect((PVOID)pageBase, pageEnd-pageBase,
			flOldProtect, &flOldProtect);	
	}

private:
	size_t pageBase;
	size_t pageEnd;
	DWORD flOldProtect;
};

void* callPatch(void* lpCall, void* lpNewProc)
{
	BYTE* callBase = (BYTE*)lpCall;
	UnProtect unProtect(callBase, 6);
	
	if((*(BYTE*)lpCall == 0xE8 )
	||(*(BYTE*)lpCall == 0xE9 ))
	{
		size_t lpOldCall = *(size_t*)(callBase+1);
		*(size_t*)(callBase+1) = (BYTE*)(lpNewProc)-callBase-5;
		return callBase + lpOldCall + 5;	
	}
	if( *(WORD*)lpCall == 0x15FF ) 
	{
		void* lpOldCall = **(void***)(callBase+2);
		*(WORD*)callBase = 0xE890;
		*(size_t*)(callBase+2) = (BYTE*)(lpNewProc)-callBase-6;
		return lpOldCall;
	}
	if( *(WORD*)lpCall == 0x358B ) 
	{
		void* lpOldCall = **(void***)(callBase+2);
		*(WORD*)callBase = 0xBE90;
		*(size_t*)(callBase+2) = (size_t)(lpNewProc);
		return lpOldCall;
	}
	
	// make our own jump!!
	callBase[0] = 0xE9;
	*(size_t*)(callBase+1) = (BYTE*)(lpNewProc)-callBase-5;
	return NULL;
}

void memPatch(void* dst, void* src, int len)
{
	UnProtect unProtect(dst, len);
	for(int i = 0; i < len; i++)
		((char*)dst)[i] = ((char*)src)[i];
}

void memFill(void* dst, int value, int len)
{
	UnProtect unProtect(dst, len);
	for(int i = 0; i < len; i++)
		((char*)dst)[i] = value;
}
