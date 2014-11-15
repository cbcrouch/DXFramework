//
// File:     UtilsDXF.h
// Project:  DXFramework
//
// Copyright (c) 2014 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"

namespace DXF {

	#define CheckHResultError(HR) \
	if (FAILED(HR)) { \
		OutputDebugString(TEXT("HRESULT FAILED\n")); \
		return HR; \
	}

	// NOTE: make sure all conditional and loop statements use proper scoping (curly braces)
	//       when using error checking macros
	#if (_DEBUG == TRUE)
	#define CHECK_HRESULT(HR) CheckHResultError(HR)
	#define ERROR_BOX() ErrorBox(__FUNCTION__)
	#define ERROR_EXIT() ErrorExit(__FUNCTION__)
	#else
	#define CHECK_HRESULT(hr) {}
	#define ERROR_BOX() {}
	#define ERROR_EXIT() {}
	#endif

	void ErrorBox(LPSTR lpszFunction); // warns of error using a message box
	void ErrorExit(LPSTR lpszFunction); // displays error in message box then exits

	//
	// TODO: needs better parameter names
	//
	HRESULT ANSItoUTF8(_Out_ LPWSTR lpszUtf, _In_ LPCSTR lpszAnsi);
	HRESULT UTF8toANSI(_Out_ LPSTR lpszAnsi, _In_ LPCWSTR lpszUtf);

	//
	// TODO: add timing functionality to utilsDX, while it will primarily be used for pacing
	//       but it can also be used to quickly benchmark other functions, modules, etc.
	//

	typedef struct BasicTimer_t {
		LARGE_INTEGER m_frequency;
		LARGE_INTEGER m_currentTime;
		LARGE_INTEGER m_startTime;
		LARGE_INTEGER m_lastTime;
		float m_total;
		float m_delta;
	} BasicTimer_t;

	void InitTimer(BasicTimer_t *pTimer);
	void Reset(BasicTimer_t *pTimer);
	void Update(BasicTimer_t *pTimer);
};
