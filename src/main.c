/* (C) Afonso Santos, October 2014, Portugal */

#include <pebble.h>
#include "FrameFX.h"


// UI related
static Window     *mainWindow ;
static Layer      *contentLayer ;


// World related
#define UPDATE_INTERVAL_MS  40

static int        updateCount   = 0 ;
static AppTimer  *updateTimer   = NULL ;


void
contentLayer_update_proc
( Layer    *me
, GContext *gCtx
)
{ graphics_draw_text(	gCtx
                    , "The quick brown fox jumped over the lazy dog.  But why did this happen?  Wouldn’t you think a fox, regardless of how quick he might be, would choose to run away from the dog?"
                    , fonts_get_system_font( FONT_KEY_ROBOTO_CONDENSED_21 )
                    , layer_get_bounds( me )
                    , GTextOverflowModeWordWrap
                    , GTextAlignmentLeft
                    , NULL
                    )	;

  static AccelData ad ;
  accel_service_peek( &ad ) ;

  switch ( (++updateCount / 100) % 6 )    // Every 100 frames/updates swich wich frameFX function is being demoed.
  { case 0:
      if ( !ad.did_vibrate ) frameFX_rotate( gCtx, ad.x / 7, -ad.y / 6 ) ;
      break ;

    case 1:
      frameFX_flipHorizontal( gCtx ) ;
      break ;

    case 2:
      frameFX_flipVertical( gCtx ) ;
      break ;

    case 3:
      frameFX_invert( gCtx ) ;
      frameFX_rotateVertical( gCtx, -updateCount ) ;
      break ;

    case 4:
      frameFX_rotateHorizontal( gCtx, -updateCount ) ;
      break ;

    case 5:
      frameFX_fillRand( gCtx ) ;
      break ;
  }
}


void
contentLayer_update_timer_handler
( void *data )
{ layer_mark_dirty( contentLayer ) ;      // this will call the contentLayer_update_proc( ) method :)
  updateTimer = app_timer_register( UPDATE_INTERVAL_MS, contentLayer_update_timer_handler, data ) ;    // Call me again.
}


// Acellerometer handlers.
void
accel_data_service_handler
( AccelData *data
, uint32_t   num_samples
)
{ }


void
mainWindow_load
( Window *window )
{ window_set_background_color( window, GColorBlack ) ;
  Layer *window_root_layer = window_get_root_layer( window ) ;

  GRect bounds = layer_get_frame( window_root_layer ) ;
  contentLayer = layer_create( bounds ) ;

  layer_set_update_proc( contentLayer, contentLayer_update_proc ) ;
  layer_add_child( window_root_layer, contentLayer ) ;
 
 	accel_data_service_subscribe( 0, accel_data_service_handler ) ;

 	contentLayer_update_timer_handler( NULL ) ;                                         // Trigger call to launch animation, will self repeat.
}


void
mainWindow_unload
( Window *window )
{ app_timer_cancel( updateTimer ) ;          // Stop animation.
  layer_destroy( contentLayer ) ;
 	accel_data_service_unsubscribe( ) ;
}


void
app_init
( void )
{ mainWindow = window_create( ) ;
 
  window_set_window_handlers( mainWindow
                            , (WindowHandlers)
                              { .load   = mainWindow_load
                              , .unload = mainWindow_unload
                              }
                            ) ;

  window_set_fullscreen( mainWindow, true ) ;
  window_stack_push( mainWindow, false ) ;
}


void
app_deinit
( void )
{ window_stack_remove( mainWindow, false ) ;
  window_destroy( mainWindow ) ;
}


int
main
( void )
{ app_init( ) ;
  app_event_loop( ) ;
  app_deinit( ) ;
}