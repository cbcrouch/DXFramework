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
        LPVOID lpMsgBuf = nullptr;
        LPVOID lpDisplayBuf = nullptr;
        DWORD dw = GetLastError();

        // NOTE: if a relevant HRESULT is needed use the following
        //HRESULT hr = HRESULT_FROM_WIN32(dw);

        // NOTE: FormatMessage will allocate memory using LocalAlloc, free with LocalFree
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, nullptr);
        assert(lpMsgBuf != nullptr);

        SIZE_T size = SizeNeededInUTF8(lpszFunction);
        HANDLE hProcHeap = GetProcessHeap();

        LPWSTR lpszFunctionW = (LPWSTR)HeapAlloc(hProcHeap, HEAP_ZERO_MEMORY, size);
        assert(lpszFunctionW != nullptr);

        HRESULT hr = ANSItoUTF8(lpszFunction, (int)size, lpszFunctionW);
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
        assert(lpDisplayBuf != nullptr);

        StringCchPrintf((LPTSTR)lpDisplayBuf, displayBufLen, TEXT("%s failed with error %d: %s"),
            lpszFunctionW, dw, (LPCTSTR)lpMsgBuf);

        MessageBox(nullptr, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

        // NOTE: FormatMessage allocated memory using LocalAlloc, free with LocalFree
        LocalFree(lpMsgBuf);

        BOOL bVal = HeapFree(hProcHeap, 0, (LPVOID)lpszFunctionW);
        bVal = HeapFree(hProcHeap, 0, lpDisplayBuf);
    }

    void ErrorExit(LPSTR lpszFunction) {
        ErrorBox(lpszFunction);
        ExitProcess(GetLastError());
    }

    SIZE_T SizeNeededInUTF8(LPCSTR lpszAnsi) {
        //
        // TODO: need to double the size needed to accommodate wide chars, something is not quite right
        //       about this (possibly using MultiByteToWideChar wrong, CP_ACP doesn't seem to be right either)
        //
        return MultiByteToWideChar(CP_UTF8, 0, lpszAnsi, -1, nullptr, 0) * 2;
    }

    SIZE_T SizeNeededInANSI(LPCWSTR lpszUtf) {
        return WideCharToMultiByte(CP_UTF8, 0, lpszUtf, -1, nullptr, 0, nullptr, FALSE);
    }

    HRESULT ANSItoUTF8(LPCSTR lpszAnsi, const int size, LPWSTR lpszUtf) {
        return MultiByteToWideChar(CP_UTF8, 0, lpszAnsi, -1, lpszUtf, size);
    }

    HRESULT UTF8toANSI(LPCWSTR lpszUtf, const int size, LPSTR lpszAnsi) {
        return WideCharToMultiByte(CP_UTF8, 0, lpszUtf, -1, lpszAnsi, size, nullptr, FALSE);
    }

    FileMemory_t ReadFileIntoMemory(LPCTSTR fileName) {
        FileMemory_t fm = {};

        HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
            DXF_ERROR_BOX();
            return fm;
        }

        // NOTE: GetFileSize is deprecated, use GetFileSizeEx instead
        LARGE_INTEGER fileSize;
        if (!GetFileSizeEx(hFile, &fileSize)) {
            DXF_ERROR_BOX();
            CloseHandle(hFile);
            return fm;
        }

        void *pMemory = malloc((size_t)fileSize.QuadPart);
        assert(pMemory != nullptr);


        //
        // TODO: defines for max values and an inline safe truncate function
        //
        assert(fileSize.QuadPart < 0xffffffff);
        DWORD fileSize32 = (DWORD)fileSize.QuadPart;


        DWORD bytesRead = 0;
        if (!ReadFile(hFile, pMemory, fileSize32, &bytesRead, nullptr)) {
            DXF_ERROR_BOX();
            CloseHandle(hFile);
            free(pMemory);
            return fm;
        }
        assert(fileSize32 == bytesRead);

        if (!CloseHandle(hFile)) {
            DXF_ERROR_BOX();
            return fm;
        }

        fm.fileSize = fileSize;
        fm.data = pMemory;

        return fm;
    }

    void DestroyFileMemory(FileMemory_t& fileMemory) {
        //
        // TODO: this pattern would also make a useful macro e.g. DXF_FREE() and for COM objects DXF_RELEASE()
        //
        if (fileMemory.data) {
            free(fileMemory.data);
            fileMemory.data = nullptr;
        }
        memset(&(fileMemory.fileSize), 0, sizeof(LARGE_INTEGER));
    }

    void InitOperationTimer(OperationTimer_t& timer) {
        //
        // NOTE: perf counter frequency is set at boot time and does not change
        //       (at least according to the documentation)
        //

        // ticks per second (note that this is NOT CPU clock cycles)
        if (!QueryPerformanceFrequency(&(timer.frequency))) {
            DXF_ERROR_BOX();
        }

        ResetOperationTimer(timer);
    }

    void ResetOperationTimer(OperationTimer_t& timer) {
        timer.cycleCountStart = __rdtsc();
        if (!QueryPerformanceCounter(&(timer.perfCounterStart))) {
            DXF_ERROR_BOX();
        }

        timer.lastCycleCount = timer.cycleCountStart;
        timer.lastPerfCounter.QuadPart = timer.perfCounterStart.QuadPart;
    }

    void Mark(OperationTimer_t& timer) {
        timer.lastCycleCount = __rdtsc();
        if (!QueryPerformanceCounter(&(timer.lastPerfCounter))) {
            DXF_ERROR_BOX();
        }
    }

    OperationSpan_t Measure(OperationTimer_t& timer) {
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

        uint64_t cyclesElapsed = endCycleCount - timer.lastCycleCount;
        result.megaCyclesElapsed = cyclesElapsed / (1000 * 1000);

        LARGE_INTEGER delta;
        delta.QuadPart = endPerfCount.QuadPart - timer.lastPerfCounter.QuadPart;
        result.msElapsed = 1000 * delta.QuadPart / timer.frequency.QuadPart;

        return result;
    }
};
