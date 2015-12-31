//
// File:     WindowDXF.h
// Project:  DXFramework
//
// Copyright (c) 2016 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"
#include "UtilsDXF.h"

#define MAX_STRING 256


namespace DXF {
    struct WindowW32_t {
        // window specific members
        HWND handle;
        HWND parentHandle;
        WNDCLASSEX winClass;
        HMENU menuHandle;

        TCHAR title[MAX_STRING];
        TCHAR className[MAX_STRING];

        POINT origin;
        SIZE winSize;

        // application specific members
        HINSTANCE hInstance;
        HANDLE hWndCloseEvt;
    };


    HRESULT InitWindowW32(WindowW32_t& win, HINSTANCE hInst, HWND hParent, const SIZE winSize, LPCTSTR szName);

    HRESULT CreateWindowW32(WindowW32_t& win);
    void DestroyWindowW32(WindowW32_t& win);

    LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
