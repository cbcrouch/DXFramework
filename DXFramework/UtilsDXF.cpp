//
// File:     UtilsDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2014 Casey Crouch. All rights reserved.
//

#include "UtilsDXF.h"

#include <strsafe.h> // for error handling

namespace DXF {

	void ErrorBox(LPSTR lpszFunction) {
		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		DWORD dw = GetLastError();

		// NOTE: if a relevant HRESULT is needed use the following
		//HRESULT hr = HRESULT_FROM_WIN32(dw);

		// NOTE: FormatMessage will allocate memory
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, lpszFunction, -1, NULL, 0);

		//
		// TODO: replace LocalAlloc with HeapAlloc/HeapFree etc. (can use HeapAlloc and start of function and HeapFree at end
		//       to clear out all dynamic memory allocations)
		//

		// need to double the size needed to accommodate wide chars
		LPWSTR lpszFunctionW = (LPWSTR)LocalAlloc(LMEM_ZEROINIT, sizeNeeded * 2);
		HRESULT hr = MultiByteToWideChar(CP_UTF8, 0, lpszFunction, -1, lpszFunctionW, sizeNeeded);

		switch (hr) {
			case ERROR_INSUFFICIENT_BUFFER: OutputDebugString(TEXT("ErrorBox: MultiByteToWideChar failed with error ERROR_INSUFFICIENT_BUFFER\n")); break;
			case ERROR_INVALID_FLAGS: OutputDebugString(TEXT("ErrorBox: MultiByteToWideChar failed with error ERROR_INVALID_FLAGS\n")); break;
			case ERROR_INVALID_PARAMETER: OutputDebugString(TEXT("ErrorBox: MultiByteToWideChar failed with error ERROR_INVALID_PARAMETER\n")); break;
			case ERROR_NO_UNICODE_TRANSLATION: OutputDebugString(TEXT("ErrorBox: MultiByteToWideChar failed with error ERROR_NO_UNICODE_TRANSLATION\n")); break;
			default: break;
		}

		//
		// TODO: need to set the displayBuffPadding to whatever the maximum DWORD error length is plus the length of the
		//       text string used in the StringCchPrintf call
		//
		const int displayBuffPadding = 64;
		lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunctionW) + displayBuffPadding) * sizeof(TCHAR));
		StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR),
			TEXT("%s failed with error %d: %s"), lpszFunctionW, dw, (LPCTSTR)lpMsgBuf);

		MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

		LocalFree(lpMsgBuf);
		LocalFree(lpszFunctionW);
		LocalFree(lpDisplayBuf);
	}

	void ErrorExit(LPSTR lpszFunction) {
		ErrorBox(lpszFunction);
		ExitProcess(GetLastError());
	}

	HRESULT ANSItoUTF8(_Out_ LPWSTR lpszUtf, _In_ LPCSTR lpszAnsi) {
		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, lpszAnsi, -1, NULL, 0);
		return MultiByteToWideChar(CP_UTF8, 0, lpszAnsi, -1, lpszUtf, sizeNeeded);
	}

	HRESULT UTF8toANSI(_Out_ LPSTR lpszAnsi, _In_ LPCWSTR lpszUtf) {
		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, lpszUtf, -1, NULL, 0, NULL, FALSE);
		return WideCharToMultiByte(CP_UTF8, 0, lpszUtf, -1, lpszAnsi, sizeNeeded, NULL, FALSE);
	}

	void InitTimer(BasicTimer_t *pTimer) {
		// ticks per second
		if (!QueryPerformanceFrequency(&(pTimer->m_frequency))) {
			ERROR_BOX();
		}
		Reset(pTimer);
	}

	void Reset(BasicTimer_t *pTimer) {
		Update(pTimer);
		pTimer->m_startTime = pTimer->m_currentTime;
		pTimer->m_total = 0.0f;
		pTimer->m_delta = 1.0f / 60.0f;
	}

	void Update(BasicTimer_t *pTimer) {
		// will get number of clock ticks
		if (!QueryPerformanceCounter(&(pTimer->m_currentTime))) {
			ERROR_BOX();
		}

		//
		// TODO: store a running average (approx. 10 frames) of how long it took to render each frame, can
		//       then use this input into adjusting the sleep value
		//

		// will also be very handy to have the resolution of the sleep used i.e. what is the maximum time
		// (or average max) that the sleep can run over in ticks


		pTimer->m_total = (float)(((double)(pTimer->m_currentTime.QuadPart - pTimer->m_startTime.QuadPart)) /
			(double)pTimer->m_frequency.QuadPart);

		if (pTimer->m_lastTime.QuadPart == pTimer->m_startTime.QuadPart) {
			// if the timer was just reset, report a time delta equivalent to 60Hz frame time
			pTimer->m_delta = 1.0f / 60.0f;
		}
		else {
			pTimer->m_delta = (float)((double)(pTimer->m_currentTime.QuadPart - pTimer->m_lastTime.QuadPart) /
				(double)pTimer->m_frequency.QuadPart);
		}

		pTimer->m_lastTime = pTimer->m_currentTime;
	}
};
