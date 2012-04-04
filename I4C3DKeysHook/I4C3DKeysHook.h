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

	DLL_EXPORT BOOL AddHookedKeyCode( UINT uKeyCode );
	DLL_EXPORT void RemoveHookedKeyCode( UINT uKeyCode );
	DLL_EXPORT BOOL IsKeyDown( UINT uKeyCode );
	DLL_EXPORT BOOL IsAllKeysDown();

#ifdef __cplusplus
}
#endif
