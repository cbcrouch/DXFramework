//
// File:     WindowDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#include "WindowDXF.h"

namespace DXF {

	LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		LRESULT rv = 0;
		WindowW32_t *pWin = (WindowW32_t *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

		switch (msg) {
			case WM_SIZE: {
				OutputDebugString(TEXT("DXFramework: WM_SIZE\n"));
				//
				// TODO: resize the viewport
				//
				RECT clientRect;
				GetClientRect(hwnd, &clientRect);
			} break;

			case WM_ACTIVATEAPP: {
				OutputDebugString(TEXT("DXFramework: WM_ACTIVATEAPP\n"));
				//
				// TODO: should pause the simulation/rendering/etc. when window is no longer active
				//
			} break;

			case WM_NCCREATE: {
				// this message will be recieved right before the WM_CREATE message and its lParam
				// is a pointer to the CREATESTRUCT which contains the application provided data
				// pointer in lpCreateParams which is needed to bind the user data to the window handle
				OutputDebugString(TEXT("DXFramework: WM_NCCREATE\n"));
				pWin = (WindowW32_t *)(((CREATESTRUCT *)lParam)->lpCreateParams);
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pWin);
				rv = TRUE;
			} break;

			case WM_CREATE: {
				OutputDebugString(TEXT("DXFramework: WM_CREATE\n"));
				//
				// TODO: create/set the condition variable to trigger the device creation or emit message (benchmark for fastest implementation)
				//
			} break;

			case WM_CLOSE: {
				OutputDebugString(TEXT("DXFramework: WM_CLOSE\n"));
				// user has clicked the close button

				// signal the render thread to close through the event system
				// wait until the render thread to exit before destroying the window

				// don't need to pulse since event set manual reset to false
				BOOL rEvt = SetEvent(pWin->hWndCloseEvt);

				// call WindowW32_t dtor then destroy the Win32 HWND
				DestroyWindowW32(pWin);
				if (DestroyWindow(pWin->handle) != TRUE) {
					OutputDebugString(TEXT("destroy window call failed\n"));
					return E_FAIL;
				}
			} break;

			case WM_DESTROY: {

				OutputDebugString(TEXT("DXFramework: WM_DESTROY\n"));

				// window has been destroyed, window handle is no longer valid at this point

				// issue quit message to break out of main message loop
				PostQuitMessage(0);
			} break;

			default: {
				rv = DefWindowProc(hwnd, msg, wParam, lParam);
			} break;
		}

		return rv;
	}

	HRESULT InitWindowW32(HINSTANCE hInst, LPCTSTR szName, SIZE winSize, HWND hParent, WindowW32_t* pWin) {
		// initialize with defaults
		pWin->handle = NULL;
		pWin->parentHandle = hParent;
		pWin->hInstance = hInst;
		pWin->menuHandle = NULL;
		pWin->origin.x = CW_USEDEFAULT;
		pWin->origin.y = CW_USEDEFAULT;
		pWin->winSize = winSize;

		// copy strings
#ifdef UNICODE
		wcsncpy_s(pWin->title, szName, MAX_STRING - 1);
		wcsncpy_s(pWin->className, L"DXWindowClass", MAX_STRING - 1);
#else
		strncpy_s(pWin->title, szName, MAX_STRING - 1);
		strncpy_s(pWin->className, "DXWindowClass", MAX_STRING - 1);
#endif

		// populate window class struct
		pWin->winClass.cbSize = sizeof(WNDCLASSEX);

		//
		// NOTE: add CS_OWNDC if planning on getting and then retaining window device context
		//
		pWin->winClass.style = CS_HREDRAW | CS_VREDRAW;

		pWin->winClass.lpfnWndProc = WindowProcedure;
		pWin->winClass.cbClsExtra = 0;
		pWin->winClass.cbWndExtra = 0;
		pWin->winClass.hInstance = pWin->hInstance;
		pWin->winClass.hIcon = LoadIcon(NULL, (LPCTSTR)IDI_APPLICATION);
		pWin->winClass.hIconSm = LoadIcon(NULL, (LPCTSTR)IDI_APPLICATION);
		pWin->winClass.hCursor = LoadCursor(NULL, (LPCTSTR)IDC_ARROW);
		pWin->winClass.hbrBackground = NULL;
		pWin->winClass.lpszMenuName = NULL;
		pWin->winClass.lpszClassName = pWin->className;

		// create an event to notify others that the window has been closed
		// default security, auto reset, false intial state, event name
		pWin->hWndCloseEvt = CreateEvent(NULL, FALSE, FALSE, TEXT("WindowW32CloseEvt"));
		if (pWin->hWndCloseEvt == NULL) {
			DXF_ERROR_BOX();
		}

		return S_OK;
	}

	HRESULT CreateWindowW32(WindowW32_t* pWin) {
		if (!RegisterClassEx(&(pWin->winClass))) {
			return E_FAIL;
		}

		DWORD dwStyle = WS_OVERLAPPEDWINDOW; // overlapped window with a title bar, border, window menu, sizing border, min/max button
		DWORD dwExStyle = WS_EX_CLIENTEDGE; // window has a border with a sunken edge

		RECT rc = { 0, 0, (LONG)(pWin->winSize.cx), (LONG)(pWin->winSize.cy) };
		AdjustWindowRect(&rc, dwStyle, FALSE);

		pWin->handle = CreateWindowEx(dwExStyle, pWin->className, pWin->title, dwStyle, pWin->origin.x, pWin->origin.y,
			rc.right - rc.left, rc.bottom - rc.top, pWin->parentHandle, pWin->menuHandle, pWin->hInstance, pWin);
		if (pWin->handle == NULL) {
			return E_FAIL;
		}

		//
		// TODO: determine why the title text is not getting shown by default and fix
		//
		SetWindowText(pWin->handle, pWin->title);
		ShowWindow(pWin->handle, SW_SHOW);

		return S_OK;
	}

	void DestroyWindowW32(WindowW32_t* pWin) {
		// NOTE: this is where any application data in the WindowW32_t struct would be cleaned up
		CloseHandle(pWin->hWndCloseEvt);

		BOOL rv = UnregisterClass((LPCTSTR)pWin->className, pWin->hInstance);
	}
};