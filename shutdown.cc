
typedef enum _SHUTDOWN_ACTION {
     ShutdownNoReboot,
     ShutdownReboot,
     ShutdownPowerOff
 } SHUTDOWN_ACTION;

extern "C"
DWORD WINAPI NtShutdownSystem (SHUTDOWN_ACTION Action);


void HandleShutdownKey()
{
	HANDLE hToken = 0;
	TOKEN_PRIVILEGES tkp = {0};
	OpenProcessToken(GetCurrentProcess(),
		TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES, &hToken);
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0);
	CloseHandle(hToken); 
	NtShutdownSystem(ShutdownReboot);
}
