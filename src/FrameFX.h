/* (C) Afonso Santos, October 2014, Portugal */

#pragma once

#include <pebble.h>


void
frameFX_fillRand    // Fill with random generated patterns.
( GContext *gCtx )
;

void
frameFX_invert
( GContext *gCtx )
;

void
frameFX_rotateVertical
( GContext *gCtx
, int16_t   rotation    // negative is up, positive is down.
)
;

void
frameFX_rotateHorizontal    // NOT YET fully implemented.
( GContext *gCtx
, int16_t   rotation    // negative is left, positive is right.
)
;

void
frameFX_flipVertical    // Bytewise version
( GContext *gCtx )
;

void
frameFX_flipHorizontal
( GContext *gCtx )
;
