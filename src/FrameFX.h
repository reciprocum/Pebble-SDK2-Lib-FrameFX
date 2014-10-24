/* (C) Afonso Santos, October 2014, Portugal */

#pragma once

#include <pebble.h>


void
frameFX_fillRand
( GContext *gCtx )
{ GBitmap *bitMap = graphics_capture_frame_buffer(	gCtx ) ;
  if (bitMap == NULL) return ;

  int *frameBuffer = (int *)bitMap->addr ;
  int numWords = bitMap->bounds.size.h * (bitMap->row_size_bytes >> 2) ;
  for( int i = 0  ;  i < numWords  ;  ++i ) *frameBuffer++ = rand( ) ;

  graphics_release_frame_buffer( gCtx, bitMap ) ;
}


void
frameFX_rotateVertical
( GContext  *gCtx
, int16_t   rotation    // negative is up, positive is down.
)
{ GBitmap *bitMap = graphics_capture_frame_buffer(	gCtx ) ;
  if (bitMap == NULL) return ;

  uint16_t absRotation = rotation < 0 ? -rotation  : rotation ;
  uint16_t numRows = bitMap->bounds.size.h ;
  if (absRotation >= numRows) absRotation %= numRows ;    // numRows == 1  will cause absRotation == 0

  if (absRotation == 0)
  { graphics_release_frame_buffer( gCtx, bitMap ) ;
    return ;  // Nothing to do.
  }

  uint8_t rowSizeWords = bitMap->row_size_bytes >> 2 ;
  int *frameBufferBase = (int *)bitMap->addr ;

  int scratchPad[numRows] ;

  // Iterate word columns.
  for( uint8_t iCol = 0  ;  iCol < rowSizeWords  ;  ++iCol )
  { int *colBase = frameBufferBase + iCol ;
    int *colWord = colBase ;

    // Save a copy of the col's content prior to rotating it.
    for( uint16_t iRow = 0  ;  iRow < numRows  ;  ++iRow )
    { scratchPad[iRow] = *colWord ;
      colWord += rowSizeWords ;
    }

    // Do the rotation.
    uint16_t sRow = rotation < 0 ? absRotation : numRows - absRotation ;
    colWord = colBase ;

    for( uint16_t iRow = 0  ;  iRow < numRows  ;  ++iRow )
    { *colWord = scratchPad[sRow] ;
      colWord += rowSizeWords ;            // Advance column word pointer to next row.
      if (++sRow == numRows) sRow = 0 ;    // Advance (and wrap around) source index to next row.
    }
  }

  graphics_release_frame_buffer( gCtx, bitMap ) ;
}


void
frameFX_rotateHorizontal
( GContext *gCtx )
{ GBitmap *bitMap = graphics_capture_frame_buffer(	gCtx ) ;
  if (bitMap == NULL) return ;

  // TODO: everything.
 
  graphics_release_frame_buffer( gCtx, bitMap ) ;
}


void
frameFX_flipVertical
( GContext *gCtx )
{ GBitmap *bitMap = graphics_capture_frame_buffer(	gCtx ) ;
  if (bitMap == NULL) return ;

  // TODO: everything.
 
  graphics_release_frame_buffer( gCtx, bitMap ) ;
}


void
frameFX_flipHorizontal
( GContext *gCtx )
{ GBitmap *bitMap = graphics_capture_frame_buffer(	gCtx ) ;
  if (bitMap == NULL) return ;

  // TODO: everything.
 
  graphics_release_frame_buffer( gCtx, bitMap ) ;
}
