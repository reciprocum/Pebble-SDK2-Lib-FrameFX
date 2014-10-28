/* (C) Afonso Santos, October 2014, Portugal */

#pragma once

#include <pebble.h>


// Fill with random generated patterns.
void frameFX_fillRand( GContext *gCtx ) ;

// Black pixels turn white, white ones turn black.
void frameFX_invert( GContext *gCtx ) ;

// Mirror image.
void frameFX_flipVertical( GContext *gCtx ) ;

// Image is turned upside down.
void frameFX_flipHorizontal( GContext *gCtx ) ;

// Negative rotation is up, positive is down.
void frameFX_rotateVertical( GContext *gCtx, int16_t rotation ) ;

// Negative rotation is left, positive is right.
void frameFX_rotateHorizontal( GContext *gCtx, int16_t rotation ) ;     // NOT YET fully implemented.
