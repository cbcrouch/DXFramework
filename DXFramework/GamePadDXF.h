//
// File:     GamePadDXF.h
// Project:  DXFramework
//
// Copyright (c) 2014 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"

//
// TODO: implement stub
//

namespace DXF {
	typedef struct GamePads_t GamePads_t;

	void createGamePads(_Out_ GamePads_t **ppGamePads);
	void initGamePads(_In_ GamePads_t *pGamePads);
	void destroyGamePads(_Out_ GamePads_t **ppGamePads);

	void activatePolling(_In_ GamePads_t *pGamePads);
	void stopPolling(_In_ GamePads_t *pGamePads);
};
