//
// File:     GamePadDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#include "GamePadDXF.h"


namespace DXF {

    //
    // TODO: if possible remove the create/destroy functions, allocation of memory should
    //       be handled external to the module
    //
    void createGamePads(GamePads_t** ppGamePads) {
        //
    }

    void destroyGamePads(GamePads_t** ppGamePads) {
        //
    }



    void initGamePads(GamePads_t& gamePads) {
        //
    }

    void activatePolling(GamePads_t& gamePads) {
        //
        // TODO: will launch a thread which will poll the controller state at a given rate
        //       and queue up events based on the controller state changes
        //
    }

    void stopPolling(GamePads_t& gamePads) {
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
