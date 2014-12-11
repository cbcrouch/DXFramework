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

		// decrement by number of format specifier characters since they will be replaced in the final string
		displayBufLen -= 6;

		// calculate number of digits dw will print
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

	void InitOperationTimer(OperationTimer_t *pTimer) {
		//
		// NOTE: perf counter frequency is set at boot time and does not change
		//       (at least according to the documentation)
		//

		// ticks per second (note that this is NOT CPU clock cycles)
		if (!QueryPerformanceFrequency(&(pTimer->frequency))) {
			DXF_ERROR_BOX();
		}

		ResetOperationTimer(pTimer);
	}

	void ResetOperationTimer(OperationTimer_t *pTimer) {
		pTimer->cycleCountStart = __rdtsc();
		if (!QueryPerformanceCounter(&(pTimer->perfCounterStart))) {
			DXF_ERROR_BOX();
		}

		pTimer->lastCycleCount = pTimer->cycleCountStart;
		pTimer->lastPerfCounter.QuadPart = pTimer->perfCounterStart.QuadPart;
	}

	void Mark(OperationTimer_t *pTimer) {
		pTimer->lastCycleCount = __rdtsc();
		if (!QueryPerformanceCounter(&(pTimer->lastPerfCounter))) {
			DXF_ERROR_BOX();
		}
	}

	OperationSpan_t Measure(OperationTimer_t *pTimer) {
		OperationSpan_t result;

		uint64_t endCycleCount = __rdtsc();

		LARGE_INTEGER endPerfCount;
		if (!QueryPerformanceCounter(&endPerfCount)) {
			DXF_ERROR_BOX();
		}

		//
		// TODO: need to verify calculations
		//

		uint64_t cyclesElapsed = endCycleCount - pTimer->lastCycleCount;
		result.megaCyclesElapsed = cyclesElapsed / (1000 * 1000);

		LARGE_INTEGER delta;
		delta.QuadPart = endPerfCount.QuadPart - pTimer->lastPerfCounter.QuadPart;
		result.microSecondsElapsed = (1000 * 1000 * delta.QuadPart) / pTimer->frequency.QuadPart;

		return result;
	}
};
