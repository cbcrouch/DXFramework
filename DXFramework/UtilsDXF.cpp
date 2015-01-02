//
// File:     UtilsDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
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

	FileMemory_t ReadFileIntoMemory(_In_ LPCTSTR fileName) {
		FileMemory_t fm;
		ZeroMemory(&fm, sizeof(FileMemory_t));


		//
		// TODO: should this use FILE_READ_DATA or GENERIC_READ ??
		//
		//HANDLE hFile = CreateFile(fileName, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);


		if (hFile == INVALID_HANDLE_VALUE) {
			DXF_ERROR_BOX();
		}

		//
		// TODO: need to restructure how the error checking works in order to make this less branchy and generally cleaner
		//

		// NOTE: GetFileSize is deprecated, use GetFileSizeEx instead
		LARGE_INTEGER fileSize;
		if (!GetFileSizeEx(hFile, &fileSize)) {
			DXF_ERROR_BOX();
		}
		else {
			//void *pMemory = VirtualAlloc(fileSize.QuadPart);
			void *pMemory = malloc(fileSize.QuadPart);
			if (pMemory) {
				DWORD bytesRead = 0;
				//
				// TODO: defines for max values and an inline safe truncate function
				//
				assert(fileSize.QuadPart < 0xffffffff);
				DWORD fileSize32 = (DWORD)fileSize.QuadPart;

				if (!ReadFile(hFile, pMemory, fileSize32, &bytesRead, NULL)) {
					if (pMemory) {
						free(pMemory);
						pMemory = NULL;
					}
					DXF_ERROR_BOX();
				}

				assert(fileSize32 == bytesRead);
			}

			fm.fileSize = fileSize;
			fm.data = pMemory;
		}

		CloseHandle(hFile);
		return fm;
	}

	void DestroyFileMemory(_Inout_ FileMemory_t *pFileMemory) {
		//
		// TODO: this pattern would also make a useful macro e.g. DXF_FREE() and for COM objects DXF_RELEASE()
		//
		if (pFileMemory->data) {
			free(pFileMemory->data);
		}

		memset(&(pFileMemory->fileSize), 0, sizeof(LARGE_INTEGER));
		pFileMemory->data = NULL;
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

		//
		// TODO: may also want to try GetTickCount64
		//
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
		result.msElapsed = 1000 * delta.QuadPart / pTimer->frequency.QuadPart;

		return result;
	}
};
