//
// File:     EventsDXF.h
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//


#pragma once


// altdevblogaday.com/2011/07/03/threading-and-your-game-loop
// update thread
// prepare thread
// render thread
// (this would have two frames of latency...)

// should focus on batching updates


// typical game loop:
// check for user input
// handle network IO (optional)
// run AI -> generate/modify state
// run physics sim -> generate/modify pose
// resolve collisions -> output modified pose + state
// update object positions and animations -> finalize pose/animation (prepare thread)
// draw graphics -> modify primitives according to pose + state      (render thread)
// play sounds -> some triggers will respond to state changes


// primitives stored in scene pool(s) / heap(s)
// pose and state travel through event system into queues



//
// TODO: need to implement an event subsystem and shared memory pool (with permissions)
//       that is integrated into the task subsystem
//

typedef enum WINDOW_EVENT : unsigned int {
	WND_CLOSE = 0,
	WND_CREATE
} WINDOW_EVENT;

//
// TODO: make these of TCHAR type
//
const wchar_t* WINDOW_EVENT_STR[] = {
	L"WindowW32CloseEvt",
	L"WindowW32CreateEvt"  // window successfully created, it is now safe to create the device
};

// WINDOW_EVENT_STR[WND_CLOSE] == L"WindowW32CloseEvt"

//
// TODO: modules that support events should have their events loaded after creation with a explicit register events call
//       i.e. call sequence would be InitX(...), CreateX(...), RegisterEventsX(...), application running, DestroyX(...)
//

//
// TODO: open/register async event callback function
//
/*
HANDLE hWaitObject = NULL; // this would be return value
HANDLE rEvt = OpenEvent(EVENT_ALL_ACCESS, TRUE, TEXT("WindowW32CloseEvt"));
if (rEvt != NULL) {
	// async wait using Windows thread pool
	RegisterWaitForSingleObject(&hWaitObject, rEvt, RenderFlagCallback, (PVOID)(&running),
		INFINITE, WT_EXECUTEONLYONCE);
}
*/

// example callback
/*
VOID CALLBACK RenderFlagCallback(_In_ PVOID lpParam, _In_ BOOLEAN timerWaitOrFired) {
	*((PBOOL)lpParam) = FALSE;
}
*/

//
// TODO: close/exit event function
//
/*
// TODO: lookup how to properly clean up the event we're waiting on
if (hWaitObject != NULL) {
	UnregisterWait(hWaitObject);
}

//CloseHandle(hWaitObject);
//CloseHandle(rEvt);
*/

//
// TODO: refer to notes at the top of DXFramework.cpp and get together a candidate list of events
//
typedef enum RENDER_EVENT : unsigned int {
	FRAME_READY = 0
} RENDER_EVENT;

//
// TODO: make this of type TCHAR
//
const wchar_t* RENDER_EVENT_STR[] = {
	L"RenderFrameEvt"
};

typedef enum CONTROL_EVENT : unsigned int {
	PAUSE_SIM = 0
} CONTROL_EVENT;

const wchar_t* CONTROL_EVENT_STR[] = {
	L"PauseSimEvt"
};

typedef enum SYSTEM_EVENT : unsigned int {
	SYS_EXIT = 0
} SYSTEM_EVENT;

const wchar_t* SYS_EVENT_STR[] = {
	L"SysExitEvt"
};
