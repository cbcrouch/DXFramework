//
// File:     WindowDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2016 Casey Crouch. All rights reserved.
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
                DestroyWindowW32(*pWin);
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


            //
            // TODO: use WM_SETCURSOR to disable the cursor or load a custom one on the fly based on
            //       some program state
            //

            //case WM_SETCURSOR: {
            //	SetCursor(hCursor);
            //} break;


                //
                // TODO: when switching to fullscreen mode will want to call ChangeDisplaySettings
                //


            default: {
                rv = DefWindowProc(hwnd, msg, wParam, lParam);
            } break;
        }

        return rv;
    }

    HRESULT InitWindowW32(WindowW32_t& win, HINSTANCE hInst, HWND hParent, const SIZE winSize, LPCTSTR szName) {
        // initialize with defaults
        win.handle = nullptr;
        win.parentHandle = hParent;
        win.hInstance = hInst;
        win.menuHandle = nullptr;
        win.origin.x = CW_USEDEFAULT;
        win.origin.y = CW_USEDEFAULT;
        win.winSize = winSize;

        // copy strings
#ifdef UNICODE
        wcsncpy_s(win.title, szName, MAX_STRING - 1);
        wcsncpy_s(win.className, L"DXWindowClass", MAX_STRING - 1);
#else
        strncpy_s(win.title, szName, MAX_STRING - 1);
        strncpy_s(win.className, "DXWindowClass", MAX_STRING - 1);
#endif

        // populate window class struct
        win.winClass.cbSize = sizeof(WNDCLASSEX);

        //
        // NOTE: add CS_OWNDC if planning on getting and then retaining window device context
        //
        win.winClass.style = CS_HREDRAW | CS_VREDRAW;

        win.winClass.lpfnWndProc = WindowProcedure;
        win.winClass.cbClsExtra = 0;
        win.winClass.cbWndExtra = 0;
        win.winClass.hInstance = win.hInstance;
        win.winClass.hIcon = LoadIcon(nullptr, (LPCTSTR)IDI_APPLICATION);
        win.winClass.hIconSm = LoadIcon(nullptr, (LPCTSTR)IDI_APPLICATION);

        //
        // TODO: load and store any additionally needed cursors, will require a cursors array in
        //       the WindowW32_t structure
        //
        win.winClass.hCursor = LoadCursor(nullptr, (LPCTSTR)IDC_ARROW);

        win.winClass.hbrBackground = nullptr;
        win.winClass.lpszMenuName = nullptr;
        win.winClass.lpszClassName = win.className;

        // create an event to notify others that the window has been closed
        // default security, auto reset, false intial state, event name
        win.hWndCloseEvt = CreateEvent(nullptr, FALSE, FALSE, TEXT("WindowW32CloseEvt"));
        if (win.hWndCloseEvt == nullptr) {
            DXF_ERROR_BOX();
        }

        return S_OK;
    }

    HRESULT CreateWindowW32(WindowW32_t& win) {
        if (!RegisterClassEx(&(win.winClass))) {
            return E_FAIL;
        }

        DWORD dwStyle = WS_OVERLAPPEDWINDOW; // overlapped window with a title bar, border, window menu, sizing border, min/max button
        DWORD dwExStyle = WS_EX_CLIENTEDGE; // window has a border with a sunken edge

        RECT rc = { 0, 0, (LONG)(win.winSize.cx), (LONG)(win.winSize.cy) };
        AdjustWindowRect(&rc, dwStyle, FALSE);

        win.handle = CreateWindowEx(dwExStyle, win.className, win.title, dwStyle, win.origin.x, win.origin.y,
            rc.right - rc.left, rc.bottom - rc.top, win.parentHandle, win.menuHandle, win.hInstance, &win);
        if (win.handle == nullptr) {
            return E_FAIL;
        }

        //
        // TODO: determine why the title text is not getting shown by default and fix
        //
        SetWindowText(win.handle, win.title);
        ShowWindow(win.handle, SW_SHOW);

        return S_OK;
    }

    void DestroyWindowW32(WindowW32_t& win) {
        // NOTE: this is where any application data in the WindowW32_t struct would be cleaned up
        CloseHandle(win.hWndCloseEvt);

        BOOL rv = UnregisterClass((LPCTSTR)win.className, win.hInstance);
    }
};