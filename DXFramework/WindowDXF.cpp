//
// File:     WindowDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2014 Casey Crouch. All rights reserved.
//

#include "WindowDXF.h"
#include "EventsDXF.h"


namespace DXF {

	// NOTE: all windows will publish the following events
	//
	// TODO: problem with this is that the events will overlap (i.e. will be the same strings for all windows)
	//       should think of a good way to modify the string by appending a GUID on the end or something to
	//       make them unique, though would then need an accessor function to return the unique event strings
	//       for a given window handle in order to register for the events elsewhere
	//
	const WINDOW_EVENT evts[] = {
		WND_CLOSE,
		WND_CREATE
	};


	LRESULT CALLBACK WindowProcedure(_In_ HWND hwnd, _In_ UINT msg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
		LRESULT rv = 0;
		static WindowW32_t *pWin = (WindowW32_t *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

		if (msg == WM_NCCREATE) { // non-client create
			pWin = (WindowW32_t *)(((CREATESTRUCT *)lParam)->lpCreateParams);
			pWin->handle = hwnd;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pWin);
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		if (!pWin) {
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		switch (msg) {
			//
			// TODO: add support for XInput (for gamepad control) perhaps include in a separate file gamepadDX.cpp
			//

			//case WM_KEYDOWN:
		case WM_KEYUP:

			if (wParam == VK_SPACE)
				OutputDebugString(TEXT("spacebar pressed\n"));

			// TODO: emit message to the rendering thread to pause the animation

			break;

		case WM_CREATE:

			//
			// TODO: create/set the condition variable to trigger the device creation or emit message (benchmark for fastest implementation)
			//

			//
			// TODO: setup for in-engine console
			//       use DirectWrite http://msdn.microsoft.com/en-us/library/windows/desktop/dd368038(v=vs.85).aspx
			//
			//TEXTMETRIC tm;
			//HDC hdc;
			//hdc = GetDC(hwnd);
			//GetTextMetrics(hdc, &tm);
			//ReleaseDC(hwnd, hdc);

			// ...

			// TODO: see MSDN example for additional information
			// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646268(v=vs.85).aspx

			break;

		case WM_CLOSE: {
			// signal the render thread to close through the event system
			// wait until the render thread to exit before destroying the window

			// don't need to pulse since event set manual reset to false
			BOOL rEvt = SetEvent(pWin->hWndCloseEvt);

			// call WindowW32_t dtor then destroy the Win32 HWND
			DestroyWindowW32(pWin);
			BOOL rWin = DestroyWindow(pWin->handle);
			if (rWin == FALSE) {
				OutputDebugString(TEXT("destroy window call failed\n"));
				return E_FAIL;
			}
		}
			break;

		case WM_DESTROY:
			// issue quit message to break out of main message loop
			PostQuitMessage(0);
			break;

			//
			// TODO: handle keyboard input, link keypresses to a triggered event
			//

		default:
			rv = DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}

		return rv;
	}

	HRESULT InitWindowW32(_Out_ WindowW32_t *pWin, _In_ HINSTANCE hInst, _In_z_ LPCTSTR szName, _In_ SIZE winSize, _In_opt_ HWND hParent) {
		// initialize with defaults
		pWin->handle = NULL;
		pWin->parentHandle = hParent;
		pWin->hInstance = hInst;
		pWin->menuHandle = NULL;

		pWin->dwStyle = WS_OVERLAPPEDWINDOW; // overlapped window with a title bar, border, window menu, sizing border, min/max button
		pWin->dwExStyle = WS_EX_CLIENTEDGE; // window has a border with a sunken edge

		pWin->origin.x = CW_USEDEFAULT;
		pWin->origin.y = CW_USEDEFAULT;
		pWin->winSize = winSize;


		// copy strings

		//
		// TODO: make sure that these are the correct string copy functions to use
		//
		ua_tcscpy_s(pWin->title, MAX_STRING - 1, szName);
		ua_tcscpy_s(pWin->className, MAX_STRING - 1, TEXT("DXWindowClass"));

		//wcsncpy_s(pWin->title, pName, MAX_STRING - 1);
		//wcsncpy_s(pWin->className, L"DXWindowClass", MAX_STRING - 1);


		// populate window class struct
		pWin->winClass.cbSize = sizeof(WNDCLASSEX);
		pWin->winClass.style = WS_OVERLAPPED;
		pWin->winClass.lpfnWndProc = WindowProcedure;
		pWin->winClass.cbClsExtra = 0;
		pWin->winClass.cbWndExtra = 0;
		pWin->winClass.hInstance = pWin->hInstance;
		pWin->winClass.hIcon = LoadIcon(NULL, (LPCTSTR)IDI_APPLICATION);
		pWin->winClass.hIconSm = LoadIcon(NULL, (LPCTSTR)IDI_APPLICATION);
		pWin->winClass.hCursor = LoadCursor(NULL, (LPCTSTR)IDC_ARROW);
		pWin->winClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		pWin->winClass.lpszMenuName = NULL;
		pWin->winClass.lpszClassName = pWin->className;

		// create an event to notify others that the window has been closed
		// default security, auto reset, false intial state, event name
		pWin->hWndCloseEvt = CreateEvent(NULL, FALSE, FALSE, TEXT("WindowW32CloseEvt"));
		if (pWin->hWndCloseEvt == NULL) {
			ERROR_BOX();
		}

		return S_OK;
	}

	HRESULT CreateWindowW32(_Inout_ WindowW32_t *pWin) {
		if (!RegisterClassEx(&(pWin->winClass))) {
			return E_FAIL;
		}

		RECT rc = { 0, 0, (LONG)(pWin->winSize.cx), (LONG)(pWin->winSize.cy) };
		AdjustWindowRect(&rc, pWin->dwStyle, FALSE);

		pWin->handle = CreateWindowEx(pWin->dwExStyle, pWin->className, pWin->title, pWin->dwStyle, pWin->origin.x, pWin->origin.y,
			rc.right - rc.left, rc.bottom - rc.top, pWin->parentHandle, pWin->menuHandle, pWin->hInstance, pWin);
		if (pWin->handle == NULL) {
			return E_FAIL;
		}

		ShowWindow(pWin->handle, SW_SHOW);
		return S_OK;
	}

	void DestroyWindowW32(_Inout_ WindowW32_t *pWin) {
		// NOTE: this is where any application data in the WindowW32_t struct would be cleaned up
		CloseHandle(pWin->hWndCloseEvt);

		BOOL rv = UnregisterClass((LPCTSTR)pWin->className, pWin->hInstance);
	}
};