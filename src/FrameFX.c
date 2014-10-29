/* (C) Afonso Santos, October 2014, Portugal */

#include <pebble.h>


/*** GBitmap functions ***/

// Fill with random generated patterns. Half-word wise variant because rand() only generates 31 bits of random data at a time.
void
frameFX_GBitmap_fillRand
( GBitmap *bitMap )
{ uint16_t *ptrFrameBufferHalfWord = (uint16_t *)bitMap->addr ;
  uint16_t numHalfWords = bitMap->bounds.size.h * (bitMap->row_size_bytes >> 1) ;
  for( uint16_t iHalfWord = 0  ;  iHalfWord < numHalfWords  ;  ++iHalfWord ) *ptrFrameBufferHalfWord++ = rand( ) ;
}


// Black pixels turn white, white pixels turn black.
void
frameFX_GBitmap_invert
( GBitmap *bitMap )
{ uint32_t *ptrFrameBufferWord = (uint32_t *)bitMap->addr ;
  uint16_t numWords = bitMap->bounds.size.h * (bitMap->row_size_bytes >> 2) ;

  for( uint16_t iWord = 0  ;  iWord < numWords  ;  ++iWord )
  { uint32_t frameBufferWord = *ptrFrameBufferWord ;
    *ptrFrameBufferWord++ = ~frameBufferWord ;
  }
}


void
frameFX_GBitmap_rotateVertical
( GBitmap *bitMap
, int16_t  rotation        // Negative rotation is upwards, positive is downwards.
)
{ uint16_t absRotation = rotation < 0 ? -rotation  : rotation ;
  uint16_t numRows = bitMap->bounds.size.h ;
  if (absRotation >= numRows) absRotation %= numRows ;                              // numRows == 1  will cause absRotation == 0
  if (absRotation == 0) return ;                                                    // Nothing to do.

  uint16_t rowSpanWords = bitMap->row_size_bytes >> 2 ;
  uint32_t *ptrFrameBufferBase = (uint32_t *)bitMap->addr ;

  uint32_t original[numRows] ;
  uint16_t srcRowInitialIdx = rotation < 0 ? absRotation : numRows - absRotation ;  // Transform an up rotation into an equivalent down rotation.

  // Iterate word columns.
  for( uint16_t iCol = 0  ;  iCol < rowSpanWords  ;  ++iCol )
  { uint32_t *ptrColBase = ptrFrameBufferBase + iCol ;
    uint32_t *ptrColWord = ptrColBase ;

    // Save a copy of the original col's content prior to rotating it.
    for( uint16_t idxRow = 0  ;  idxRow < numRows  ;  ++idxRow )
    { original[idxRow] = *ptrColWord ;
      ptrColWord += rowSpanWords ;
    }

    // Do the rotation.
    uint16_t srcRowIdx = srcRowInitialIdx ;
    ptrColWord = ptrColBase ;

    for( uint16_t idxRow = 0  ;  idxRow < numRows  ;  ++idxRow )
    { *ptrColWord = original[srcRowIdx] ;
      ptrColWord += rowSpanWords ;                    // Advance column word pointer to next row.
      if (++srcRowIdx == numRows) srcRowIdx = 0 ;     // Advance (and wrap around) source row index to next row.
    }
  }
}


void
frameFX_GBitmap_rotateHorizontal
( GBitmap *bitMap
, int16_t  rotation        // Negative rotation is leftwards, positive is rightwards.
)
{ uint16_t absRotation = rotation < 0 ? -rotation : rotation ;
  uint16_t rowSizeBits = bitMap->bounds.size.w -1 ;               // Not quite clear why bounds reported width is 1 bit bigger than the actual image.
  if (absRotation >= rowSizeBits) absRotation %= rowSizeBits ;    // rowSizeBits == 1  will cause absRotation == 0
  if (absRotation == 0) return ;                                  // Nothing to do.
  if (rotation < 0) absRotation = rowSizeBits - absRotation ;     // Convert a left rotation into an equivalent right rotation.

  uint16_t numRows         = bitMap->bounds.size.h ;
  uint16_t rowSizeBytes    = bitMap->row_size_bytes ;
  uint8_t  srcRowByte[rowSizeBytes] ;
  uint8_t *ptrRowLeftmostByte = (uint8_t *)bitMap->addr ;

  // Iterate rows.
  for ( uint16_t rowIdx = 0  ;  rowIdx < numRows  ;  ++rowIdx )
  { uint8_t *ptrRowByte = ptrRowLeftmostByte ;

    // Save a copy of the original row's content prior to rotating it.
    for ( uint16_t byteOffset = 0  ;  byteOffset < rowSizeBytes  ;  ++byteOffset )  srcRowByte[byteOffset] = *ptrRowByte++ ;

    // Row right shifting with rotation of the right outflowing bits.
    uint16_t srcRowBitIdx = rowSizeBits - absRotation ;

    // Scan the row bit by bit from left to right.
    for( uint16_t tgtRowBitIdx = 0  ;  tgtRowBitIdx < rowSizeBits  ;  ++tgtRowBitIdx )
    { uint8_t *ptrTgtByte = ptrRowLeftmostByte + (tgtRowBitIdx >> 3) ;

      if (srcRowByte[srcRowBitIdx >> 3] & (1 << (srcRowBitIdx & 0b111)))
        *ptrTgtByte |= 1 << (tgtRowBitIdx & 0b111) ;           // Source bit was 1 so set the target bit.
      else
        *ptrTgtByte &= ~(1 << (tgtRowBitIdx & 0b111)) ;        // Source bit was 0 so clear the target bit.

      if (++srcRowBitIdx > rowSizeBits)  srcRowBitIdx = 0 ;    // Move the source bit index to the right and wrap-around if needed.
    }
  
    ptrRowLeftmostByte += rowSizeBytes ;  // Advance one row down.
  }
}


void
frameFX_GBitmap_rotate
( GBitmap *bitMap
, int16_t  horizontal       // Negative is leftwards, positive is rightwards.
, int16_t  vertical         // Negative is upwards, positive is downwards.
)
{ frameFX_GBitmap_rotateHorizontal( bitMap, horizontal ) ;
  frameFX_GBitmap_rotateVertical( bitMap, vertical ) ;
}


// Mirror image.
void
frameFX_GBitmap_flipVertical
( GBitmap *bitMap )
{ uint16_t numRows      = bitMap->bounds.size.h ;
  uint16_t rowSizeBytes = bitMap->row_size_bytes ;

  uint8_t *ptrLeftmostByte = (uint8_t *)bitMap->addr - 1 ;    // The "-1" is required due to a bug on the bitMap->addr value that points 1 byte too far inside the frame buffer.

  // Iterate rows.
  for( uint16_t rowIdx = 0  ;  rowIdx < numRows  ;  ++rowIdx )
  { uint8_t *ptrLeftByte  = ptrLeftmostByte ;
    uint8_t *ptrRightByte = ptrLeftmostByte + rowSizeBytes - 1 ;

    while (ptrLeftByte <= ptrRightByte)
    { uint8_t leftByte  = *ptrLeftByte ;                // Read the original frame buffer bit content.
      uint8_t rightByte = *ptrRightByte ;

      // Revert left byte bits.
      __asm( "rbit %[theByte], %[theByte]          \n"  // The rotation instruction is only available as a 32 bit word variant.
             "mov  %[theByte], %[theByte], ror #24 \n"  // Bring the leftmost 8 bits (of the rotation result) to the rightmost 8 bits by shifting them right 24 bits.
           : [theByte] "+r" (leftByte)                  // Output/Input: the byte to be reverted.
           ) ;

      // Revert right byte bits.
      __asm( "rbit %[theByte], %[theByte]          \n"  // The rotation instruction is only available as a 32 bit word variant.
             "mov  %[theByte], %[theByte], ror #24 \n"  // Bring the leftmost 8 bits (of the rotation result) to the rightmost 8 bits by shifting them right 24 bits.
           : [theByte] "+r" (rightByte)                 // Output/Input: the byte to be reverted.
           ) ;

      // Swap store the reverted content and move the left/right pointers closer towards each other.
      *ptrLeftByte++  = rightByte ;
      *ptrRightByte-- = leftByte  ;
    }

    ptrLeftmostByte += rowSizeBytes ;  // Advance one row down.
  }
}


// Image is turned upside down.
void
frameFX_GBitmap_flipHorizontal
( GBitmap *bitMap )
{ uint32_t *ptrFrameBufferBase = (uint32_t *)bitMap->addr ;
  uint16_t rowSpanWords = bitMap->row_size_bytes >> 2 ;
  uint16_t colSpanWords = (bitMap->bounds.size.h - 1) * rowSpanWords ;

  // Iterate word columns.
  for( uint16_t rowOffset = 0  ;  rowOffset < rowSpanWords  ;  ++rowOffset )
  { uint32_t *ptrTopColWord    = ptrFrameBufferBase + rowOffset ;
    uint32_t *ptrBottomColWord = ptrTopColWord + colSpanWords ;

    while (ptrTopColWord < ptrBottomColWord)
    { uint32_t swapWord = *ptrTopColWord ; *ptrTopColWord = *ptrBottomColWord ; *ptrBottomColWord = swapWord ;
      ptrTopColWord    += rowSpanWords ;
      ptrBottomColWord -= rowSpanWords ;
    }
  }
}


/*** GContext functions ***/

void
frameFX_fillRand
( GContext *gCtx )
{ GBitmap *bitMap = graphics_capture_frame_buffer( gCtx ) ;
  if (bitMap == NULL) return ;
  frameFX_GBitmap_fillRand( bitMap ) ;
  graphics_release_frame_buffer( gCtx, bitMap ) ;
}

void
frameFX_invert
( GContext *gCtx )
{ GBitmap *bitMap = graphics_capture_frame_buffer( gCtx ) ;
  if (bitMap == NULL) return ;
  frameFX_GBitmap_invert( bitMap ) ;
  graphics_release_frame_buffer( gCtx, bitMap ) ;
}

void
frameFX_rotateVertical
( GContext *gCtx
, int16_t   rotation        // Negative rotation is upwards, positive is downwards.
)
{ GBitmap *bitMap = graphics_capture_frame_buffer( gCtx ) ;
  if (bitMap == NULL) return ;
  frameFX_GBitmap_rotateVertical( bitMap, rotation ) ;
  graphics_release_frame_buffer( gCtx, bitMap ) ;
}

void
frameFX_rotateHorizontal
( GContext *gCtx
, int16_t   rotation        // Negative rotation is leftwards, positive is rightwards.
)
{ GBitmap *bitMap = graphics_capture_frame_buffer( gCtx ) ;
  if (bitMap == NULL) return ;
  frameFX_GBitmap_rotateHorizontal( bitMap, rotation ) ;
  graphics_release_frame_buffer( gCtx, bitMap ) ;
}

void
frameFX_rotate
( GContext *gCtx
, int16_t   horizontal       // Negative is leftwards, positive is rightwards.
, int16_t   vertical         // Negative is upwards, positive is downwards.
)
{ GBitmap *bitMap = graphics_capture_frame_buffer( gCtx ) ;
  if (bitMap == NULL) return ;
  frameFX_GBitmap_rotate( bitMap, horizontal, vertical ) ;
  graphics_release_frame_buffer( gCtx, bitMap ) ;
}

void
frameFX_flipVertical
( GContext *gCtx )
{ GBitmap *bitMap = graphics_capture_frame_buffer( gCtx ) ;
  if (bitMap == NULL) return ;
  frameFX_GBitmap_flipVertical( bitMap ) ;
  graphics_release_frame_buffer( gCtx, bitMap ) ;
}

void
frameFX_flipHorizontal
( GContext *gCtx )
{ GBitmap *bitMap = graphics_capture_frame_buffer( gCtx ) ;
  if (bitMap == NULL) return ;
  frameFX_GBitmap_flipHorizontal( bitMap ) ;
  graphics_release_frame_buffer( gCtx, bitMap ) ;
}
