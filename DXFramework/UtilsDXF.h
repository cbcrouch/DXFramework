//
// File:     UtilsDXF.h
// Project:  DXFramework
//
// Copyright (c) 2016 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"

#define DXF_CheckHResultError(HR) \
if (FAILED(HR)) { \
    OutputDebugString(TEXT("HRESULT FAILED\n")); \
    return HR; \
}

// NOTE: make sure all conditional and loop statements use proper scoping
//       when using error checking macros (all conditionals/loops need curlys)
#if (_DEBUG == TRUE)
#define DXF_CHECK_HRESULT(HR) DXF_CheckHResultError(HR)
#define DXF_ERROR_BOX() DXF::ErrorBox(__FUNCTION__)
#define DXF_ERROR_EXIT() DXF::ErrorExit(__FUNCTION__)
#else
#define DXF_CHECK_HRESULT(hr) {}
#define DXF_ERROR_BOX() {}
#define DXF_ERROR_EXIT() {}
#endif

//
// TODO: define custom "assert" macro that will do this for internal builds and then strip out
//       for shipping builds
//
//#define DXF_ASSERT(cond) if(!cond) { DXF_ERROR_BOX(); }
//DXF_ASSERT(hFile != INVALID_HANDLE_VALUE);


namespace DXF {
    void ErrorBox(LPSTR lpszFunction); // warns of error using a message box
    void ErrorExit(LPSTR lpszFunction); // displays error in message box then exits

    SIZE_T SizeNeededInUTF8(LPCSTR lpszAnsi);
    SIZE_T SizeNeededInANSI(LPCWSTR lpszUtf);
    HRESULT ANSItoUTF8(LPCSTR lpszAnsi, const int size, LPWSTR lpszUtf);
    HRESULT UTF8toANSI(LPCWSTR lpszUtf, const int size, LPSTR lpszAnsi);

    //inline uint32_t SafeTruncate(uint64_t value) {
    //	assert(value <= 0xffffffff);
    //	uint32_t ret = (uint32_t)value;
    //	return ret;
    //}

    struct FileMemory_t {
        LARGE_INTEGER fileSize;
        LPVOID data;
    };

    FileMemory_t ReadFileIntoMemory(LPCTSTR fileName);
    void DestroyFileMemory(FileMemory_t& fileMemory);

    struct OperationTimer_t {
        LARGE_INTEGER frequency;
        LARGE_INTEGER perfCounterStart;
        LARGE_INTEGER lastPerfCounter;
        uint64_t cycleCountStart;
        uint64_t lastCycleCount;
    };

    struct OperationSpan_t {
        int64_t msElapsed;
        int64_t megaCyclesElapsed;
    };

    void InitOperationTimer(OperationTimer_t& timer);  // gets the frequency and then sets start values
    void ResetOperationTimer(OperationTimer_t& timer); // will set the start values to current time

    // starting point which to base measurements off of
    void Mark(OperationTimer_t& timer);

    // calculate and return an OperationSpan_t (won't modify the last values in the timer)
    OperationSpan_t Measure(OperationTimer_t& timer);

    //
    // TODO: add another timer based utility that is a circular buffer of
    //       operation spans and generates statistics, can be used to smooth
    //       out frame rate based displays and to provide some automated
    //       detection of frame rate spikes/drops
    //
};
