call exmod.bat
gcc %NDK% -I%1 taskbar.cc win7keys.cc %CFLAGS2% -c
exe_mod %1\explorer_org.exe %1\explorer.exe %1\explorer.def taskbar.o win7keys.o -lexshit -lntdll
del *.o
