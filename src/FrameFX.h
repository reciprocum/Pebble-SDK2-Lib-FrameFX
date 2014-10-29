/* (C) Afonso Santos, October 2014, Portugal */

#pragma once

#include <pebble.h>


/*** GContext functions ***/

// Fill with random generated patterns.
void frameFX_fillRand( GContext *gCtx ) ;

// Black pixels turn white, white pixels turn black.
void frameFX_invert( GContext *gCtx ) ;

// Mirror image.
void frameFX_flipVertical( GContext *gCtx ) ;

// Image is turned upside down.
void frameFX_flipHorizontal( GContext *gCtx ) ;

// Negative rotation is upwards, positive is downwards.
void frameFX_rotateVertical( GContext *gCtx, int16_t rotation ) ;

// Negative rotation is leftwards, positive is rightwards.
void frameFX_rotateHorizontal( GContext *gCtx, int16_t rotation ) ;

// Negative horizontal is leftwards, positive is rightwards.
// Negative vertical is upwards, positive is downwards.
void frameFX_rotate( GContext *gCtx, int16_t horizontal, int16_t vertical ) ;


/*** GBitmap functions ***/

void frameFX_GBitmap_fillRand( GBitmap *bitMap ) ;

// Black pixels turn white, white pixels turn black.
void frameFX_GBitmap_invert( GBitmap *bitMap ) ;

// Mirror image.
void frameFX_GBitmap_flipVertical( GBitmap *bitMap ) ;

// Image is turned upside down.
void frameFX_GBitmap_flipHorizontal( GBitmap *bitMap ) ;

// Negative rotation is upwards, positive is downwards.
void frameFX_GBitmap_rotateVertical( GBitmap *bitMap, int16_t rotation ) ;

// Negative rotation is leftwards, positive is rightwards.
void frameFX_GBitmap_rotateHorizontal( GBitmap *bitMap, int16_t rotation ) ;

// Negative horizontal is leftwards, positive is rightwards.
// Negative vertical is upwards, positive is downwards.
void frameFX_GBitmap_rotate( GBitmap *bitMap, int16_t horizontal, int16_t vertical ) ;
