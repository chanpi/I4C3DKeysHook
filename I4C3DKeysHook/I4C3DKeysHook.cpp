/**
 * I4C3DKeysHook.cpp
 * 
 * Copyright (c) 2008 by <your name/ organization here>
 */
// I4C3DKeysHook.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "I4C3DKeysHook.h"

HINSTANCE g_hInstDll;

namespace {
	struct HookedKey {
		UINT uKey;
		BOOL isUp;
	};
	static const INT MAX_KEYCODES = 256;

	static HWND g_hTargetWnd;
	static HHOOK g_hHook;

	static BOOL g_bStart = FALSE;
	static HookedKey g_uKeysEntry[ MAX_KEYCODES ];
	static INT g_uKeysEntryCount = 0;

	static int g_CalledCount = 0;
}

static LRESULT CALLBACK KeyHookProc(int code, WPARAM wParam, LPARAM lParam);

/**
 * @brief
 * 該当キーが押下されたかどうかを調べます。
 * 
 * @param code
 * Description of parameter code.
 * 
 * @param wParam
 * Description of parameter wParam.
 * 
 * @param lParam
 * Description of parameter lParam.
 * 
 * @returns
 * CallNextHookEx()の結果を返します。
 * 
 * 登録した監視キーが押下（WM_KEYDOWN/WM_SYSKEYDOWN）されたかどうかを調べ、記録します。
 * 押下された場合には設定したウィンドウハンドルに通知メッセージを受け取ることができます。
 * 
 * @remarks
 * 押下された場合のメッセージを受け取らない場合は、IsKeyDown関数で調べることができます。
 * 
 * @see
 * IsKeyDown()
 */
LRESULT CALLBACK KeyHookProc(int code, WPARAM wParam, LPARAM lParam)
{
	if ( code >= 0 && code == HC_ACTION ) {
		for ( int i = 0; i < g_uKeysEntryCount; ++i ) {
			if ( wParam == g_uKeysEntry[ i ].uKey ) {
				if ( lParam < 0 ) {
					g_uKeysEntry[ i ].isUp = TRUE;
				} else { // KeyDown
					g_uKeysEntry[ i ].isUp = FALSE;
					PostMessage( g_hTargetWnd, MY_NOTIFYMESSAGE, g_uKeysEntry[ i ].uKey, 0 );
				}
			}
		}
	}
	return CallNextHookEx( g_hHook, code, wParam, lParam );
}

/**
 * @brief
 * キーボードフックを開始します。
 * 
 * @param hWnd
 * キーボードフックが起きた場合にPostMessageで通知するターゲットウィンドウ。
 * 
 * @returns
 * フックが開始できた場合にTRUE、できなかった場合にFALSEを返します。
 * 
 * キーボードフックを開始します。
 * 
 * @remarks
 * MakeHookが成功した場合は、必ずUnHookを呼び出してください。
 * 
 * @see
 * UnHook()
 */
BOOL MakeHook( HWND hWnd )
{
	if ( g_bStart ) {
		g_CalledCount++;
		return TRUE;
	}

	g_hTargetWnd = hWnd;

	g_hHook = SetWindowsHookEx( WH_KEYBOARD, ( HOOKPROC )KeyHookProc, g_hInstDll, 0 );
	if ( g_hHook == NULL ) {
		return FALSE;
	}

	g_bStart = TRUE;
	g_CalledCount++;
	return TRUE;
}

/**
 * @brief
 * キーボードフックを終了します。
 * 
 * @throws <exception class>
 * Description of criteria for throwing this exception.
 * 
 * キーボードフックを終了します。
 * 
 * @remarks
 * MakeHookが成功した場合は、必ずUnHookを呼び出してください。
 * 
 * @see
 * MakeHook()
 */
void UnHook()
{
	if ( g_hHook != NULL ) {
		g_CalledCount--;
		if (g_CalledCount <= 0) {
			UnhookWindowsHookEx( g_hHook );
			g_hHook = NULL;
		}
	}
	g_bStart = FALSE;
}

/**
 * @brief
 * フック対象のキーコードを登録します。
 * 
 * @param uKeyCode
 * フック対象のキーコード。
 * 
 * @returns
 * 登録が成功するとTRUE返します。登録上限数を超えた場合にはFALSEを返します。
 * 
 * フック対象のキーコードを複数登録できます。最大数は256です。
 * 
 * @remarks
 * RemoveHookedKeyCode()で登録を解除できます。
 * 
 * @see
 * RemoveHookedKeyCode()
 */
BOOL AddHookedKeyCode( UINT uKeyCode )
{
	if ( g_uKeysEntryCount >= MAX_KEYCODES ) {
		return FALSE;
	}
	for (int i = 0;i < g_uKeysEntryCount; ++i) {
		if (g_uKeysEntry[i].uKey == uKeyCode) {
			return TRUE;
		}
	}
	g_uKeysEntry[ g_uKeysEntryCount ].uKey = uKeyCode;
	g_uKeysEntry[ g_uKeysEntryCount++ ].isUp = TRUE;
	return TRUE;
}

/**
 * @brief
 * フック対象として登録したキーを解除します。
 * 
 * @param uKeyCode
 * フック解除対象のキーコード。
 * 
 * AddHookedKeyCode()で登録したフック対象キーコードを解除する場合に使用します。
 * 登録されていないキーが指定された場合には無視されます。
 * 
 * @remarks
 * AddHookedKeyCode()でフック対象のキーコードを登録できます。
 * 
 * @see
 * AddHookedKeyCode()
 */
void RemoveHookedKeyCode( UINT uKeyCode )
{
	for ( int i = 0; i < g_uKeysEntryCount; ++i ) {
		if ( g_uKeysEntry[ i ].uKey == uKeyCode ) {
			g_uKeysEntry[ i ] = g_uKeysEntry[ g_uKeysEntryCount-1 ];
			g_uKeysEntryCount--;
			return;
		}
	}
}


/**
 * @brief
 * 引数に指定したキーコードが押下されているかどうか返します。
 * 
 * @param uKeyCode
 * 対象のキーコード。
 * 
 * @returns
 * 引数に指定されたキーが押下されている場合にはTRUE、押下されていない場合にはFALSEを返します。
 * 
 * 引数に指定したキーコードが押下されているかどうか返します。
 * 
 * @remarks
 * 登録したすべてのキーコードを調べるにはIsAllKeysDown()を使います。
 * 
 * @see
 * IsAllKeysDown()
 */
#include <stdio.h>
BOOL IsKeyDown( UINT uKeyCode )
{
	{
		char szBuf[32];
		sprintf_s(szBuf, 32, "%d\n", g_uKeysEntryCount);
		OutputDebugStringA(szBuf);
	}

	for ( int i = 0; i < g_uKeysEntryCount; ++i ) {
		{
			char szBuf[32];
			sprintf_s(szBuf, 32, "0x%x\n", g_uKeysEntry[ i ].uKey);
			OutputDebugStringA(szBuf);
		}
		if ( g_uKeysEntry[ i ].uKey == uKeyCode && !g_uKeysEntry[ i ].isUp ) {
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * @brief
 * 登録したすべてのキーコードが押下されているかを返します。
 * 
 * @returns
 * 登録したすべてのキーが押下されている場合にはTRUE、押下されていない場合にはFALSEを返します。
 * 
 * 登録したすべてのキーコードが押下されているかを返します。
 * 
 * @remarks
 * 個別のキーコードを調べるにはIsKeyDown()を使います。
 * 
 * @see
 * IsKeyDown()
 */
BOOL IsAllKeysDown()
{
	for ( int i = 0; i < g_uKeysEntryCount; ++i ) {
		if ( g_uKeysEntry[ i ].isUp ) {
			return FALSE;
		}
	}
	return TRUE;
}