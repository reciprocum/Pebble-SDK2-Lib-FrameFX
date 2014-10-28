/* (C) Afonso Santos, October 2014, Portugal */

#pragma once

#include <pebble.h>


void
frameFX_fillRand    // Fill with random generated patterns, Half-word wise variant because rand() only generates 31 bits of random data at a time.
( GContext *gCtx )
{ GBitmap *bitMap = graphics_capture_frame_buffer(	gCtx ) ;
  if (bitMap == NULL) return ;

  uint16_t *ptrFrameBufferHalfWord = (uint16_t *)bitMap->addr ;
  uint16_t numHalfWords = bitMap->bounds.size.h * (bitMap->row_size_bytes >> 1) ;
  for( uint16_t iHalfWord = 0  ;  iHalfWord < numHalfWords  ;  ++iHalfWord ) *ptrFrameBufferHalfWord++ = rand( ) ;

  graphics_release_frame_buffer( gCtx, bitMap ) ;
}


void
frameFX_fillRandW    // Fill with random generated patterns, Word wise variant.
( GContext *gCtx )
{ GBitmap *bitMap = graphics_capture_frame_buffer(	gCtx ) ;
  if (bitMap == NULL) return ;

  uint32_t *ptrFrameBufferWord = (uint32_t *)bitMap->addr ;
  uint16_t numWords = bitMap->bounds.size.h * (bitMap->row_size_bytes >> 2) ;
  for( uint16_t i = 0  ;  i < numWords  ;  ++i ) *ptrFrameBufferWord++ = rand( ) ;

  graphics_release_frame_buffer( gCtx, bitMap ) ;
}


void
frameFX_rotateVertical
( GContext *gCtx
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

  uint8_t rowSpanWords = bitMap->row_size_bytes >> 2 ;
  uint32_t *ptrFrameBufferBase = (uint32_t *)bitMap->addr ;

  uint32_t scratchPad[numRows] ;

  // Iterate word columns.
  for( uint8_t iCol = 0  ;  iCol < rowSpanWords  ;  ++iCol )
  { uint32_t *ptrColBase = ptrFrameBufferBase + iCol ;
    uint32_t *ptrColWord = ptrColBase ;

    // Save a copy of the col's content prior to rotating it.
    for( uint16_t iRow = 0  ;  iRow < numRows  ;  ++iRow )
    { scratchPad[iRow] = *ptrColWord ;
      ptrColWord += rowSpanWords ;
    }

    // Do the rotation.
    uint16_t sRow = rotation < 0 ? absRotation : numRows - absRotation ;  // Transform an up rotation into an equivalent down rotation.
    ptrColWord = ptrColBase ;

    for( uint16_t iRow = 0  ;  iRow < numRows  ;  ++iRow )
    { *ptrColWord = scratchPad[sRow] ;
      ptrColWord += rowSpanWords ;            // Advance column word pointer to next row.
      if (++sRow == numRows) sRow = 0 ;    // Advance (and wrap around) source index to next row.
    }
  }

  graphics_release_frame_buffer( gCtx, bitMap ) ;
}


void
frameFX_rotateHorizontal    // NOT YET fully implemented.
( GContext *gCtx
, int16_t   rotation    // negative is left, positive is right.
)
{ GBitmap *bitMap = graphics_capture_frame_buffer(	gCtx ) ;
  if (bitMap == NULL) return ;

  uint16_t absRotation = rotation < 0 ? -rotation : rotation ;
  uint16_t numCols = bitMap->bounds.size.w ;
  if (absRotation >= numCols) absRotation %= numCols ;    // numCols == 1  will cause absRotation == 0

  if (absRotation == 0)
  { graphics_release_frame_buffer( gCtx, bitMap ) ;
    return ;  // Nothing to do.
  }

  if (rotation < 0)  absRotation = numCols - absRotation ;  // Convert a left rotation into an equivalent right rotation.

  // Rotate right by absRotation bits.
  uint32_t *ptrFrameBufferWord = (uint32_t *)bitMap->addr ;
  uint16_t numWords = bitMap->bounds.size.h * (bitMap->row_size_bytes >> 2) ;

  // TODO: the bulk bit right shifting with rotation of the right outflowing bits.

  graphics_release_frame_buffer( gCtx, bitMap ) ;
}


void
frameFX_flipVertical    // Bytewise version
( GContext *gCtx )
{ GBitmap *bitMap = graphics_capture_frame_buffer(	gCtx ) ;
  if (bitMap == NULL) return ;

  uint16_t numRows = bitMap->bounds.size.h ;
  uint16_t rowSpanBytes = bitMap->row_size_bytes ;

  uint8_t *ptrLeftmostByte = (uint8_t *)bitMap->addr - 1 ;    // The "-1" is required due to a bug on the bitMap->addr value that points 1 byte too far inside the frame buffer.

  // Iterate rows.
  for( uint8_t iRow = 0  ;  iRow < numRows  ;  ++iRow )
  { uint8_t *ptrLeftByte  = ptrLeftmostByte ;
    uint8_t *ptrRightByte = ptrLeftmostByte + rowSpanBytes - 1 ;

    while (ptrLeftByte <= ptrRightByte)
    { uint8_t leftByte  = *ptrLeftByte ;                  // Read the original content.
      uint8_t rightByte = *ptrRightByte ;

      // Revert left byte bits.
      __asm( "rbit %[reverteByte], %[reverteByte]          \n"  // The rotation instruction is only available as a 32bit word variant.
             "mov  %[reverteByte], %[reverteByte], ror #24 \n"  // Bring the leftmost 8 bits to the rightmost 8 bit position.
           : [reverteByte] "+r" (leftByte)                      // Output/Input: the byte to be reverted.
           ) ;

      // Revert right byte bits.
      __asm( "rbit %[reverteByte], %[reverteByte]          \n"  // The rotation instruction is only available as a 32bit word variant.
             "mov  %[reverteByte], %[reverteByte], ror #24 \n"  // Bring the leftmost 8 bits to the rightmost 8 bit position.
           : [reverteByte] "+r" (rightByte)                     // Output/Input: the byte to be reverted.
           ) ;

      *ptrLeftByte++  = rightByte ;                        // Swap store the reverted content.
      *ptrRightByte-- = leftByte  ;
    }

    ptrLeftmostByte += rowSpanBytes ;  // Advance one row down.
  }
 
  graphics_release_frame_buffer( gCtx, bitMap ) ;
}


void
frameFX_flipHorizontal
( GContext *gCtx )
{ GBitmap *bitMap = graphics_capture_frame_buffer(	gCtx ) ;
  if (bitMap == NULL) return ;

  uint32_t *ptrFrameBufferBase = (uint32_t *)bitMap->addr ;

  uint8_t  rowSpanWords = bitMap->row_size_bytes >> 2 ;
  uint16_t colSpanWords = (bitMap->bounds.size.h - 1) * rowSpanWords ;

  // Iterate word columns.
  for( uint8_t iCol = 0  ;  iCol < rowSpanWords  ;  ++iCol )
  { uint32_t *ptrTopColWord    = ptrFrameBufferBase + iCol ;
    uint32_t *ptrBottomColWord = ptrTopColWord + colSpanWords ;

    while (ptrTopColWord < ptrBottomColWord)
    { uint32_t swap = *ptrTopColWord ; *ptrTopColWord = *ptrBottomColWord ; *ptrBottomColWord = swap ;
      ptrTopColWord    += rowSpanWords ;
      ptrBottomColWord -= rowSpanWords ;
    }
  }

  graphics_release_frame_buffer( gCtx, bitMap ) ;
}
