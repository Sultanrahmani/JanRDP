#include <winpr/wlog.h>

wLog* WLog_Get(LPCSTR name)
{
  return &name;
}
BOOL WLog_IsLevelActive(wLog* _log, DWORD _log_level)
{
  return TRUE;
}

BOOL WLog_PrintMessage(wLog* log, DWORD type, DWORD level, DWORD line, const char* file,
                       const char* function, ...)
{
	return TRUE;
}
BOOL WLog_OpenAppender(wLog* log)
{
	return TRUE;
}