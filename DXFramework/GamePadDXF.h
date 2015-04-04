//
// File:     GamePadDXF.h
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
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

    void createGamePads(GamePads_t** ppGamePads);
    void initGamePads(GamePads_t* pGamePads);
    void destroyGamePads(GamePads_t** ppGamePads);

    void activatePolling(GamePads_t* pGamePads);
    void stopPolling(GamePads_t* pGamePads);
};
