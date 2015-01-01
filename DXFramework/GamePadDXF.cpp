//
// File:     GamePadDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#include "GamePadDXF.h"

//
// TODO: don't force the user to have to have XInput on their system, load the functions
//       manually if they wish to use a controller (see Handmade Hero D6 23:00), remember
//       to remove the XInput.lib linker dependency after making the change
//
#include <Xinput.h>


namespace DXF {

	typedef struct GamePads_t {
		// ...
		BOOL activeControllers[XUSER_MAX_COUNT];
	} GamePads_t;


	void createGamePads(_Out_ GamePads_t **ppGamePads) {
		//
	}

	void initGamePads(_In_ GamePads_t *pGamePads) {
		//
	}

	void destroyGamePads(_Out_ GamePads_t **ppGamePads) {
		//
	}

	void activatePolling(_In_ GamePads_t *pGamePads) {
		//
		// TODO: will launch a thread which will poll the controller state at a given rate
		//       and queue up events based on the controller state changes
		//
	}

	void stopPolling(_In_ GamePads_t *pGamePads) {
		//
	}


	void getControlerState() {
		for (int i = 0; i < XUSER_MAX_COUNT; ++i) {
			XINPUT_STATE controllerState;

			//
			// TODO: setup to only poll controllers which are connected since XInputGetState will stall
			//       when attempting to get state from a controller that is not connected
			//
			if (XInputGetState(i, &controllerState) == ERROR_SUCCESS) {
				// controller is plugged in

				//
				// TODO: check XINPUT_STATE
				//

				XINPUT_GAMEPAD *pad = &(controllerState.Gamepad);

				BOOL buttonA = pad->wButtons & XINPUT_GAMEPAD_A;
				//...
			}
			else {
				// controller is not connected

				//
				// TODO: keep a list of the controllers connected and if one that is active
				//       gets disconnected then pause simulation and ask user to reconnect
				//
			}

		}
	}
}
