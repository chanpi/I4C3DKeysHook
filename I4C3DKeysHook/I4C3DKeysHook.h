#pragma once

#include <Windows.h>

#undef DLL_EXPORT

#ifdef I4C3DKEYSHOOK_EXPORTS
#define DLL_EXPORT		__declspec(dllexport)
#else
#define DLL_EXPORT		__declspec(dllimport)
#endif

const UINT MY_NOTIFYMESSAGE = ( WM_APP + 1 );

#ifdef __cplusplus
extern "C" {
#endif
	
	DLL_EXPORT BOOL MakeHook( HWND hWnd );
	DLL_EXPORT void UnHook();

	BOOL AddHookedKeyCode( UINT uKeyCode );
	void RemoveHookedKeyCode( UINT uKeyCode );
	BOOL IsKeyDown( UINT uKeyCode );
	BOOL IsAllKeysDown();

#ifdef __cplusplus
}
#endif
