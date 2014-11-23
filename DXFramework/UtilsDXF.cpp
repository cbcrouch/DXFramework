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

		// NOTE: FormatMessage will allocate memory using LocalAlloc, free with LocalFree
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

		
		// TODO: for the entity system use the LFH (low fragmentation heap)

		// http://msdn.microsoft.com/en-us/library/windows/desktop/aa366750(v=vs.85).aspx

		
		SIZE_T size = SizeNeededInUTF8(lpszFunction);
		HANDLE hProcHeap = GetProcessHeap();

		LPWSTR lpszFunctionW = (LPWSTR)HeapAlloc(hProcHeap, HEAP_ZERO_MEMORY, size);
		assert(lpszFunctionW != NULL);

		HRESULT hr = ANSItoUTF8(lpszFunctionW, lpszFunction, (int)size);
		switch (hr) {
			case ERROR_INSUFFICIENT_BUFFER: OutputDebugString(TEXT("ErrorBox: MultiByteToWideChar failed with error ERROR_INSUFFICIENT_BUFFER\n")); break;
			case ERROR_INVALID_FLAGS: OutputDebugString(TEXT("ErrorBox: MultiByteToWideChar failed with error ERROR_INVALID_FLAGS\n")); break;
			case ERROR_INVALID_PARAMETER: OutputDebugString(TEXT("ErrorBox: MultiByteToWideChar failed with error ERROR_INVALID_PARAMETER\n")); break;
			case ERROR_NO_UNICODE_TRANSLATION: OutputDebugString(TEXT("ErrorBox: MultiByteToWideChar failed with error ERROR_NO_UNICODE_TRANSLATION\n")); break;
			default: break;
		}

		int displayBufLen = lstrlen(TEXT("%s failed with error %d: %s")) +
			lstrlen((LPCTSTR)lpszFunctionW) +
			lstrlen((LPCTSTR)lpMsgBuf);

		// decrement format specifier characters since they will be replaced in the final string
		displayBufLen -= 6;

		if (dw != 0) {
			displayBufLen += ((int)log10(dw)) + 1;
		}
		else {
			displayBufLen += 1;
		}

		lpDisplayBuf = HeapAlloc(hProcHeap, HEAP_ZERO_MEMORY, displayBufLen * sizeof(TCHAR));
		assert(lpDisplayBuf != NULL);

		StringCchPrintf((LPTSTR)lpDisplayBuf, displayBufLen, TEXT("%s failed with error %d: %s"),
			lpszFunctionW, dw, (LPCTSTR)lpMsgBuf);

		MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

		// NOTE: FormatMessage allocated memory using LocalAlloc, free with LocalFree
		LocalFree(lpMsgBuf);

		BOOL bVal = HeapFree(hProcHeap, 0, (LPVOID)lpszFunctionW);
		bVal = HeapFree(hProcHeap, 0, lpDisplayBuf);
	}

	void ErrorExit(LPSTR lpszFunction) {
		ErrorBox(lpszFunction);
		ExitProcess(GetLastError());
	}

	SIZE_T SizeNeededInUTF8(_In_ LPCSTR lpszAnsi) {
		//
		// TODO: need to double the size needed to accommodate wide chars, something is not quite right
		//       about this (possibly using MultiByteToWideChar wrong, CP_ACP doesn't seem to be right either)
		//
		return MultiByteToWideChar(CP_UTF8, 0, lpszAnsi, -1, NULL, 0) * 2;
	}

	SIZE_T SizeNeededInANSI(_In_ LPCWSTR lpszUtf) {
		return WideCharToMultiByte(CP_UTF8, 0, lpszUtf, -1, NULL, 0, NULL, FALSE);
	}

	HRESULT ANSItoUTF8(_Out_ LPWSTR lpszUtf, _In_ LPCSTR lpszAnsi, _In_ const int size) {
		return MultiByteToWideChar(CP_UTF8, 0, lpszAnsi, -1, lpszUtf, size);
	}

	HRESULT UTF8toANSI(_Out_ LPSTR lpszAnsi, _In_ LPCWSTR lpszUtf, _In_ const int size) {
		return WideCharToMultiByte(CP_UTF8, 0, lpszUtf, -1, lpszAnsi, size, NULL, FALSE);
	}

	void InitTimer(BasicTimer_t *pTimer) {
		// ticks per second
		if (!QueryPerformanceFrequency(&(pTimer->m_frequency))) {
			DXF_ERROR_BOX();
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
			DXF_ERROR_BOX();
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
