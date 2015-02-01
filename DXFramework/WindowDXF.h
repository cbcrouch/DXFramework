//
// File:     WindowDXF.h
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
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

	HRESULT InitWindowW32(HINSTANCE hInst, LPCTSTR szName, SIZE winSize, HWND hParent, WindowW32_t* pWin);

	HRESULT CreateWindowW32(WindowW32_t* pWin);
	void DestroyWindowW32(WindowW32_t* pWin);

	LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
