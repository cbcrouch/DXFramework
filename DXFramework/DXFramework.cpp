//
// File:     DXFramework.cpp
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#include "DXFCommon.h"

// base operating system
#include <process.h>  // threading
#include <mmsystem.h> // timer

#include <stdio.h> // sprintf_s

// project headers
#include "WindowDXF.h"
#include "GamePadDXF.h"
#include "RendererDXF.h"
#include "ResourceDXF.h"
#include "SamplerDXF.h"
#include "ShaderDXF.h"


//
// TODO: remove globals after devising good method for submitting data to render thread
//
DXF::ProgramDX_t g_hlslProgram;
DXF::ConstantsDX_t g_hlslConstants;

DXF::PrimitivesDX_t g_meshPrimitives;
DXF::SamplerDX_t g_hlslSampler;

CDXUTSDKMesh g_mesh11;



// MSDN Win32 Sleep
// to increase the accuracy of the sleep interval, call the timeGetDevCaps function to determine
// the supported minimum timer resolution and the timeBeginPeriod function to set the timer
// resolution to its minimum. use caution when calling timeBeginPeriod, as frequent calls can
// signficantly affect the system clock, system power usage, and the scheduler. if you call
// timeBeginPeriod, call it one time early in the application and be sure to call the timeEndPeriod
// function at the very end of the application

// NOTE: SleepEx won't be necessary since any IO callbacks should be handled by another thread



VOID CALLBACK RenderFlagCallback(_In_ PVOID lpParam, _In_ BOOLEAN timerWaitOrFired);

VOID CALLBACK RenderFlagCallback(_In_ PVOID lpParam, _In_ BOOLEAN timerWaitOrFired)
{
	*((PBOOL)lpParam) = FALSE;
}

unsigned __stdcall renderThread(void *pArgs)
{
	DXF::RendererDX_t *pRenderer = (DXF::RendererDX_t *)pArgs;
	BOOL running = TRUE;

	HANDLE hWaitObject = NULL;
	HANDLE rEvt = OpenEvent(EVENT_ALL_ACCESS, TRUE, TEXT("WindowW32CloseEvt"));
	if (rEvt != NULL) {
		// async wait using Windows thread pool
		RegisterWaitForSingleObject(&hWaitObject, rEvt, RenderFlagCallback, (PVOID)(&running),
			INFINITE, WT_EXECUTEONLYONCE);

		// NOTE: could use async wait to collect events in a queue (callback function would copy data in)
	}


	//
	// TODO: create an event to signal the render thread to start, open event here and block on it
	//       (after block check the render flag, if it is false then return since the application has been terminated)
	//


	DXF::ViewVolume_t viewVolume;
	DXF::InitViewVolume(pRenderer, &viewVolume);


	// update constant resources that won't vary (or vary much) from frame to frame
	DXF::CBNeverChanges_t cbNeverChanges;
	cbNeverChanges.view = XMMatrixTranspose(viewVolume.view);
	pRenderer->pImmediateContext->UpdateSubresource(g_hlslConstants.pNeverChanges, 0, NULL, &cbNeverChanges, 0, 0);

	DXF::CBChangeOnResize_t cbChangeOnResize;
	cbChangeOnResize.projection = XMMatrixTranspose(viewVolume.projection);
	pRenderer->pImmediateContext->UpdateSubresource(g_hlslConstants.pChangeOnResize, 0, NULL, &cbChangeOnResize, 0, 0);


	float t = 0.0f;
	ULONGLONG dwTimeStart = 0;
	ULONGLONG dwTimeCur = 0;

	//
	// TODO: find out the montiors actual refresh rate
	//
	int refreshRateHz = 60;
	float secondsElapsedPerFrame = 1000.0f / (float)refreshRateHz;

	//
	// TODO: implment an update loop for the simulation logic i.e. physics, AI, etc.
	//
	//int simulationRateHz = 30;
	//float secondsElapsedPerFrame = 1000.0f / (float)simulationRateHz;

	DXF::OperationTimer_t timer;
	DXF::InitOperationTimer(&timer);

	while(running) {

		DXF::Mark(&timer);

		//
		// TODO: break this basic time/step based logic out into a "simulation update" module within the framework
		//       (the simulation module will create transformations based on AI and physics model updates)
		//
		if (pRenderer->driverType == D3D_DRIVER_TYPE_REFERENCE) {
			// reference device will get a fixed rotation iteration since there could be a long pause between
			// frames as the application is most likely running with a GPU debugger
			t += (float)XM_PI * 0.0125f;
		}
		else {
			dwTimeCur = GetTickCount64();
			if (dwTimeStart == 0) {
				dwTimeStart = dwTimeCur;
			}
			t = (dwTimeCur - dwTimeStart) / 1000.0f;
		}

		XMMATRIX timedRotation = XMMatrixRotationY(t);


		ClearView(pRenderer);


		// update constant buffer that changes one per frame
		DXF::CBChangesEveryFrame_t cb;

		// rename constant buffers to be similar to what's in the example
		// - CB_VS_PER_OBJECT
		// - CB_PS_PER_OBJECT
		// - CB_PS_PER_FRAME

		// any need for a CB_VS_PER_FRAME constant buffer ??


		//
		// TODO: rotations and setup for model matrix only need to be performed once
		//
		XMVECTOR vCenter = XMVectorSet(0.25767413f, -28.503521f, 111.00689f, 0.0f);
		XMMATRIX mCenterMesh = XMMatrixTranslationFromVector(vCenter);
		XMVECTOR yAxis = XMVectorSet(0.0, 1.0, 0.0, 0.0);
		XMVECTOR xAxis = XMVectorSet(1.0, 0.0, 0.0, 0.0);
		mCenterMesh *= XMMatrixRotationAxis(yAxis, XM_PI);
		mCenterMesh *= XMMatrixRotationAxis(xAxis, XM_PIDIV2);



		// update constant buffer resources
		cb.world = XMMatrixTranspose(mCenterMesh * timedRotation); // applying hardcoded rotation transformation, this should be properly replaced
		pRenderer->pImmediateContext->UpdateSubresource(g_hlslConstants.pChangesEveryFrame, 0, NULL, &cb, 0, 0);

		// vertex shader setup
		pRenderer->pImmediateContext->VSSetShader(g_hlslProgram.pVertexShader, NULL, 0);
		pRenderer->pImmediateContext->VSSetConstantBuffers(0, 1, &(g_hlslConstants.pNeverChanges));
		pRenderer->pImmediateContext->VSSetConstantBuffers(1, 1, &(g_hlslConstants.pChangeOnResize));
		pRenderer->pImmediateContext->VSSetConstantBuffers(2, 1, &(g_hlslConstants.pChangesEveryFrame));

		// pixel shader setup
		pRenderer->pImmediateContext->PSSetShader(g_hlslProgram.pPixelShader, NULL, 0);

		// sampler setup
		pRenderer->pImmediateContext->PSSetSamplers(0, 1, &(g_hlslSampler.pSamplerState));



		// for rendering sdkmesh
		UINT Strides[1];
		UINT Offsets[1];
		ID3D11Buffer* pVB[1];
		pVB[0] = g_mesh11.GetVB11(0, 0);
		Strides[0] = (UINT)g_mesh11.GetVertexStride(0, 0);
		Offsets[0] = 0;
		pRenderer->pImmediateContext->IASetVertexBuffers(0, 1, pVB, Strides, Offsets);
		pRenderer->pImmediateContext->IASetIndexBuffer(g_mesh11.GetIB11(0), g_mesh11.GetIBFormat11(0), 0);

		// resource module will need the following functions
		// - GetIB11
		// - GetIBFormat11
		// - GetNumSubsets (subsets will need to look similiar to the SDKMESH_SUBSET struct
		// - GetPrimitiveType11
		// - GetMaterial (with subfunctions for retrieving different textures/values/etc.

		// draw all the subsets of the mesh
		SDKMESH_SUBSET* pSubset = NULL;
		D3D11_PRIMITIVE_TOPOLOGY PrimType;
		for (UINT subset = 0; subset < g_mesh11.GetNumSubsets(0); ++subset) {
			pSubset = g_mesh11.GetSubset(0, subset);

			PrimType = CDXUTSDKMesh::GetPrimitiveType11((SDKMESH_PRIMITIVE_TYPE)pSubset->PrimitiveType);

			// mesh specified
			//pRenderer->pImmediateContext->IASetPrimitiveTopology(PrimType);

			// wireframe mode
			pRenderer->pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			// material loading
			ID3D11ShaderResourceView* pDiffuseRV = g_mesh11.GetMaterial(pSubset->MaterialID)->pDiffuseRV11;
			pRenderer->pImmediateContext->PSSetShaderResources(0, 1, &pDiffuseRV);

			pRenderer->pImmediateContext->DrawIndexed((UINT)pSubset->IndexCount, 0, (UINT)pSubset->VertexStart);
		}


		//
		// TODO: implement proper pacing algorithm, calculate sleep interval based on timer results
		//
		DXF::OperationSpan_t timeSpan = DXF::Measure(&timer);

/*
		int64_t msElapsed = timeSpan.msElapsed;
		while (msElapsed < refreshRateHz) {
			//Sleep(0);
			timeSpan = DXF::Measure(&timer);
			msElapsed = timeSpan.msElapsed;
		}
*/

		PresentView(pRenderer);
		Sleep(0);
	}

	//
	// TODO: lookup how to properly clean up the event we're waiting on
	//
	if (hWaitObject != NULL) {
		if (UnregisterWait(hWaitObject) == FALSE) {
			DXF_ERROR_BOX();
		}
	}

	//CloseHandle(hWaitObject);
	//CloseHandle(rEvt);

	OutputDebugString(TEXT("exited render thread\n"));
	return 0;
	// NOTE: _endthreadex should be called automatically on return
	//_endthreadex(0);
}

int APIENTRY wWinMain (
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ int nCmdShow
)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	HRESULT hr = {};
	HANDLE hRenderThread = NULL;
	unsigned int threadId = 0;

	DXF::WindowW32_t dxWin = {};
	DXF::RendererDX_t dxRenderer = {};

	SIZE windowSize = {};
	windowSize.cx = 1280;
	windowSize.cy = 720;


	//
	// TODO: implement pacing algorithm to use min supported timer resolution (tc.wPeriodMin)
	//


	TIMECAPS tc = {};
	MMRESULT tr = TIMERR_NOCANDO;
	MMRESULT mmr = timeGetDevCaps(&tc, sizeof(tc));
	if (mmr != MMSYSERR_NOERROR) {
		// set the minimum timer resolution, in ms, for the application to the minimum supported
		tr = timeBeginPeriod(tc.wPeriodMin);
		if (tr == TIMERR_NOCANDO) {
			DXF_ERROR_BOX();
		}
	}

	if ((mmr == MMSYSERR_NOERROR) || (tr == TIMERR_NOCANDO)) {
		//
		// TODO: failed to get or set resolution of timer device, attempt to use some default pacing
		//
	}


	//
	// TODO: use virtual alloc to get a block of memory to manage memory
	//
	//void *pMemory = VirtualAlloc(size.QuadPart);


	// init and create the Win32 window
	hr = DXF::InitWindowW32(hInstance, TEXT("DirectX Framework"), windowSize, NULL, &dxWin);
	DXF_CHECK_HRESULT(hr);

	hr = DXF::CreateWindowW32(&dxWin);
	DXF_CHECK_HRESULT(hr);

	// create the Direct3D device
	hr = InitRenderer(dxWin.handle, &dxRenderer);
	DXF_CHECK_HRESULT(hr);

	hr = InitProgram(&dxRenderer, TEXT("Simple.fx"), &g_hlslProgram);
	DXF_CHECK_HRESULT(hr);

	hr = InitConstBuffers(&dxRenderer, &g_hlslConstants);
	DXF_CHECK_HRESULT(hr);

	hr = InitSampler(&dxRenderer, &g_hlslSampler);
	DXF_CHECK_HRESULT(hr);



	LPCTSTR fileName = TEXT("..\\Resources\\cube\\cube.obj");
	DXF::FileMemory_t fm = DXF::ReadFileIntoMemory(fileName);

	//
	// TODO: update the Wavefront Obj file reading to use the platform calls
	//

	DXF::DestroyFileMemory(&fm);



	// C:\dev\Media\Microsoft\Teapot\Teapot.sdkmesh
	// C:\dev\Media\Microsoft\misc\reflectsphere.sdkmesh


	// NOTE: create mesh function in this version of DXUT does not appear to support creation of adjacent indices
	hr = g_mesh11.Create(dxRenderer.pDevice, TEXT("..\\Resources\\tiny.sdkmesh"));

	//
	// TODO: scale down g_mesh11 using a DXF::CalcBoundingBox like function to fit it within a unit cube
	//


	//
	// TODO: teapot doesn't appear to have a texture, but does have a material which should be used to render it
	//
	//hr = g_mesh11.Create(dxRenderer.pDevice, TEXT("C:\\dev\\Media\\Microsoft\\Teapot\\Teapot.sdkmesh"));

	//hr = g_mesh11.Create(dxRenderer.pDevice, TEXT("C:\\dev\\Media\\Microsoft\\misc\\reflectsphere.sdkmesh"));

	DXF_CHECK_HRESULT(hr);


	//
	//
	//
	DXF::EntityDX_t grid = {};
	hr = DXF::GenerateGridXZ(&dxRenderer, 20, &grid);
	DXF_CHECK_HRESULT(hr);

	DXF::DestroyGridXZ(&grid);

	//
	// TODO: update DestroyEntity call to clean up all entity resource
	//
	//DXF::DestroyEntity(&grid);


	//
	// calc bounding box test
	//
	DXF::EntityDX_t sdkmesh = {};
	hr = DXF::LoadEntity(&dxRenderer, TEXT("..\\Resources\\tiny.sdkmesh"), &sdkmesh);
	DXF_CHECK_HRESULT(hr);

	XMVECTOR yAxis = XMVectorSet(0.0, 1.0, 0.0, 0.0);
	XMVECTOR xAxis = XMVectorSet(1.0, 0.0, 0.0, 0.0);
	sdkmesh.model *= XMMatrixRotationAxis(yAxis, XM_PI);
	sdkmesh.model *= XMMatrixRotationAxis(xAxis, XM_PIDIV2);

	DXF::MeshBounds_t bounds = DXF::CalcBoundingBox(&sdkmesh);

	char strBuffer[256] = { };
	sprintf_s(strBuffer, sizeof(strBuffer), "mesh bounds x:[%f, %f], y:[%f, %f], z:[%f, %f]\n", bounds.min_x, bounds.max_x,
		bounds.min_y, bounds.max_y, bounds.min_z, bounds.max_y);
	OutputDebugStringA(strBuffer);

	DXF::DestroyEntity(&sdkmesh);
	//
	//
	//


	// start render thread
	hRenderThread = (HANDLE)_beginthreadex(NULL, 0, renderThread, (void *)&dxRenderer, 0, &threadId);
	if (!hRenderThread) {
		return -1;
	}

	// increase render threads priority
	if (!SetThreadPriority(hRenderThread, THREAD_PRIORITY_HIGHEST)) {
		DXF_ERROR_BOX();
	}



	//
	// TODO: should have a separte message pump thread with priority THREAD_PRIORITY_ABOVE_NORMAL
	//       then block main on an EXIT event
	//

    // enter main Win32 message loop
    MSG msg = {0};
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			switch (msg.message) {
				// MSDN keyboard input
				// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646268(v=vs.85).aspx

				//
				// TODO: try using Raw Input for a similar threaded polling model as what is used
				//       by the XInput implementation
				//       http://msdn.microsoft.com/en-us/library/windows/desktop/ms645536(v=vs.85).aspx
				//

				case WM_SYSKEYDOWN: {
					//
				}
				case WM_SYSKEYUP: {
					//
				}

				case WM_KEYDOWN: {
					//
					// TODO: check lParam to see if key was previously down to avoid re-sending/emitting event
					//
					//BOOL wasDown = ((lParam & (1 << 30)) != 0);
					//BOOL isDown = ((lParam & (1 << 31));
					if (msg.wParam == VK_SPACE) {
						OutputDebugString(TEXT("space key down event\n"));
					}
				} break;

				case WM_KEYUP: {
					if (msg.wParam == VK_SPACE) {
						OutputDebugString(TEXT("spacebar key up event\n"));
						//
						// TODO: emit message to the rendering thread to pause the animation
						//
					}
				} break;

				default:
					// translates virtual-key messages into character messages
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					break;
			}
        }
    }



	// blocking wait for render thread - functions as a join
	DWORD dwResult = WaitForSingleObject(hRenderThread, INFINITE);
	if (dwResult == WAIT_OBJECT_0) {
		OutputDebugString(TEXT("render thread joined\n"));
	}


	//
	// TODO: need to destroy all D3D objects in order to truly exit cleanly
	//


	// if timeGetDevCaps succeeded and timeBeginPeriod succeeded
	if ((mmr != MMSYSERR_NOERROR) && (tr != TIMERR_NOCANDO)) {
		// clear previously set minimum timer resolution
		if (timeEndPeriod(tc.wPeriodMin) == TIMERR_NOCANDO) {
			// failed timeEndPeriod
			DXF_ERROR_BOX();
		}
	}



	g_mesh11.Destroy();


	DestroyProgram(&g_hlslProgram);
	DestroyConstBuffers(&g_hlslConstants);
	DestroySampler(&g_hlslSampler);

	// NOTE: window is destroyed when it receives a WM_CLOSE message
	DXF::DestroyRenderer(&dxRenderer);
	return 0;
}
