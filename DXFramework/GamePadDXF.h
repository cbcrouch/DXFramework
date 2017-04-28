//
// File:     GamePadDXF.h
// Project:  DXFramework
//
// Copyright (c) 2017 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"
#include "UtilsDXF.h"

//
// TODO: don't force the user to have to have XInput on their system, load the functions
//       manually if they wish to use a controller (see Handmade Hero D6 23:00), remember
//       to remove the XInput.lib linker dependency after making the change
//
#include <Xinput.h>


//
// TODO: implement stub
//

namespace DXF {
    struct GamePads_t {
        // ...
        BOOL activeControllers[XUSER_MAX_COUNT];
    };


    //
    // TODO: if possible remove the create/destroy functions, allocation of memory should
    //       be handled external to the module
    //
    void createGamePads(GamePads_t** ppGamePads);
    void destroyGamePads(GamePads_t** ppGamePads);


    void initGamePads(GamePads_t& gamePads);
    void activatePolling(GamePads_t& gamePads);
    void stopPolling(GamePads_t& gamePads);
};
