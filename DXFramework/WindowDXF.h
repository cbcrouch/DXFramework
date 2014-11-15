//
// File:     WindowDXF.h
// Project:  DXFramework
//
// Copyright (c) 2014 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"


//
// TODO: make window struct an opaque struct, though before doing this should think about
//       supporting multiple heaps (in this case ctor/dtor would take a pointer to the heap)
//
//       best bet is to provide an allocator (in this case heap) param to the create call
//       this will however change the Init, Create, use, Destory pattern since the opaque
//       struct pointer will need to be created first
//

#define MAX_STRING 256

namespace DXF {
	typedef struct WindowW32_t {
		// window specific members
		HWND handle;
		HWND parentHandle;
		WNDCLASSEX winClass;
		HMENU menuHandle;
		DWORD dwExStyle;
		DWORD dwStyle;

		TCHAR title[MAX_STRING];
		TCHAR className[MAX_STRING];

		POINT origin;
		SIZE winSize;

		// application specific members
		HINSTANCE hInstance;
		HANDLE hWndCloseEvt;
	} WindowW32_t;

	HRESULT InitWindowW32(_Out_ WindowW32_t *pWin, _In_ HINSTANCE hInst, _In_z_ LPCTSTR szName, _In_ SIZE winSize, _In_opt_ HWND hParent);

	HRESULT CreateWindowW32(_Inout_ WindowW32_t *pWin);
	void DestroyWindowW32(_Inout_ WindowW32_t *pWin);

	LRESULT CALLBACK WindowProcedure(_In_ HWND hwnd, _In_ UINT msg, _In_ WPARAM wParam, _In_ LPARAM lParam);
};
