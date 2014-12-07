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



	//
	// TODO: refactor timer so that it doesn't use/need floats
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
