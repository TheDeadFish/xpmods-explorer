#ifndef _CALLPATCH_H_
#define _CALLPATCH_H_

#define HOTCALL(ftype, addr) (*((typeof(&ftype))(size_t(addr))))
#define CALLPATCH(call, proc) callPatch((void*)(call), (void*)(proc))
void* callPatch(void* lpCall, void* lpNewProc);

void memPatch(void* dst, void* src, int len);
template <class T, int N>
void MEMPATCH(T dst, const char (& src)[N])
 {	memPatch((void*)dst, (void*)src, N-1); }
template <class T, class U>
void MEMPATCH(T dst, U src)
 {  memPatch((void*)dst, (void*)&src, 4); }
template <class T, class U>
void MEMPATCH(T dst, U src, int len)
 {  memPatch((void*)dst, (void*)src, len); }
 
void memFill(void* dst, int value, int len);
template <class T, class U>
void MEMNOP(T dst, U end)
 {  memFill((void*)dst, 0x90, int(end)-int(dst)); }

#endif
