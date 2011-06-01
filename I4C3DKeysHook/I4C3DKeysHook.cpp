/**
 * I4C3DKeysHook.cpp
 * 
 * Copyright (c) 2008 by <your name/ organization here>
 */
// I4C3DKeysHook.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
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
 * �Y���L�[���������ꂽ���ǂ����𒲂ׂ܂��B
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
 * CallNextHookEx()�̌��ʂ�Ԃ��܂��B
 * 
 * �o�^�����Ď��L�[�������iWM_KEYDOWN/WM_SYSKEYDOWN�j���ꂽ���ǂ����𒲂ׁA�L�^���܂��B
 * �������ꂽ�ꍇ�ɂ͐ݒ肵���E�B���h�E�n���h���ɒʒm���b�Z�[�W���󂯎�邱�Ƃ��ł��܂��B
 * 
 * @remarks
 * �������ꂽ�ꍇ�̃��b�Z�[�W���󂯎��Ȃ��ꍇ�́AIsKeyDown�֐��Œ��ׂ邱�Ƃ��ł��܂��B
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
 * �L�[�{�[�h�t�b�N���J�n���܂��B
 * 
 * @param hWnd
 * �L�[�{�[�h�t�b�N���N�����ꍇ��PostMessage�Œʒm����^�[�Q�b�g�E�B���h�E�B
 * 
 * @returns
 * �t�b�N���J�n�ł����ꍇ��TRUE�A�ł��Ȃ������ꍇ��FALSE��Ԃ��܂��B
 * 
 * �L�[�{�[�h�t�b�N���J�n���܂��B
 * 
 * @remarks
 * MakeHook�����������ꍇ�́A�K��UnHook���Ăяo���Ă��������B
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
 * �L�[�{�[�h�t�b�N���I�����܂��B
 * 
 * @throws <exception class>
 * Description of criteria for throwing this exception.
 * 
 * �L�[�{�[�h�t�b�N���I�����܂��B
 * 
 * @remarks
 * MakeHook�����������ꍇ�́A�K��UnHook���Ăяo���Ă��������B
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
 * �t�b�N�Ώۂ̃L�[�R�[�h��o�^���܂��B
 * 
 * @param uKeyCode
 * �t�b�N�Ώۂ̃L�[�R�[�h�B
 * 
 * @returns
 * �o�^�����������TRUE�Ԃ��܂��B�o�^������𒴂����ꍇ�ɂ�FALSE��Ԃ��܂��B
 * 
 * �t�b�N�Ώۂ̃L�[�R�[�h�𕡐��o�^�ł��܂��B�ő吔��256�ł��B
 * 
 * @remarks
 * RemoveHookedKeyCode()�œo�^�������ł��܂��B
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
 * �t�b�N�ΏۂƂ��ēo�^�����L�[���������܂��B
 * 
 * @param uKeyCode
 * �t�b�N�����Ώۂ̃L�[�R�[�h�B
 * 
 * AddHookedKeyCode()�œo�^�����t�b�N�ΏۃL�[�R�[�h����������ꍇ�Ɏg�p���܂��B
 * �o�^����Ă��Ȃ��L�[���w�肳�ꂽ�ꍇ�ɂ͖�������܂��B
 * 
 * @remarks
 * AddHookedKeyCode()�Ńt�b�N�Ώۂ̃L�[�R�[�h��o�^�ł��܂��B
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
 * �����Ɏw�肵���L�[�R�[�h����������Ă��邩�ǂ����Ԃ��܂��B
 * 
 * @param uKeyCode
 * �Ώۂ̃L�[�R�[�h�B
 * 
 * @returns
 * �����Ɏw�肳�ꂽ�L�[����������Ă���ꍇ�ɂ�TRUE�A��������Ă��Ȃ��ꍇ�ɂ�FALSE��Ԃ��܂��B
 * 
 * �����Ɏw�肵���L�[�R�[�h����������Ă��邩�ǂ����Ԃ��܂��B
 * 
 * @remarks
 * �o�^�������ׂẴL�[�R�[�h�𒲂ׂ�ɂ�IsAllKeysDown()���g���܂��B
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
 * �o�^�������ׂẴL�[�R�[�h����������Ă��邩��Ԃ��܂��B
 * 
 * @returns
 * �o�^�������ׂẴL�[����������Ă���ꍇ�ɂ�TRUE�A��������Ă��Ȃ��ꍇ�ɂ�FALSE��Ԃ��܂��B
 * 
 * �o�^�������ׂẴL�[�R�[�h����������Ă��邩��Ԃ��܂��B
 * 
 * @remarks
 * �ʂ̃L�[�R�[�h�𒲂ׂ�ɂ�IsKeyDown()���g���܂��B
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