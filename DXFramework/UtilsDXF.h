//
// File:     UtilsDXF.h
// Project:  DXFramework
//
// Copyright (c) 2014 Casey Crouch. All rights reserved.
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

namespace DXF {
	void ErrorBox(LPSTR lpszFunction); // warns of error using a message box
	void ErrorExit(LPSTR lpszFunction); // displays error in message box then exits

	SIZE_T SizeNeededInUTF8(_In_ LPCSTR lpszAnsi);
	SIZE_T SizeNeededInANSI(_In_ LPCWSTR lpszUtf);
	HRESULT ANSItoUTF8(_Out_ LPWSTR lpszUtf, _In_ LPCSTR lpszAnsi, _In_ const int size);
	HRESULT UTF8toANSI(_Out_ LPSTR lpszAnsi, _In_ LPCWSTR lpszUtf, _In_ const int size);

	typedef struct OperationTimer_t {
		LARGE_INTEGER frequency;
		LARGE_INTEGER perfCounterStart;
		LARGE_INTEGER lastPerfCounter;
		uint64_t cycleCountStart;
		uint64_t lastCycleCount;
	} OperationTimer_t;

	typedef struct OperationSpan_t {
		int64_t microSecondsElapsed;
		int64_t megaCyclesElapsed;
	} OperationSpan_t;

	void InitOperationTimer(OperationTimer_t *pTimer);  // gets the frequency and then sets start values
	void ResetOperationTimer(OperationTimer_t *pTimer); // will set the start values to current time

	// starting point which to base measurements off of
	void Mark(OperationTimer_t *pTimer);

	// calculate and return an OperationSpan_t (won't modify the last values in the timer)
	OperationSpan_t Measure(OperationTimer_t *pTimer);

	//
	// TODO: add another timer based utility that is a circular buffer of
	//       operation spans and generates statistics, can be used to smooth
	//       out frame rate based displays and to provide some automated
	//       detection of frame rate spikes/drops
	//
};
