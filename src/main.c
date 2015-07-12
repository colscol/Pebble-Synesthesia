#include <pebble.h>
#include <stdio.h>

// Apple's strTok function since pebble does not
// support the standard strtok
#include "myStrTok.h"
 
// Dict define
#define KEY_COLORS 0

// Persistant storage structure
typedef struct persist {
      int colorIdx[11];
} __attribute__((__packed__)) persist;

persist settings = {
  .colorIdx[0] = 0,
  .colorIdx[1] = 42,
  .colorIdx[2] = 48,
  .colorIdx[3] = 57,
  .colorIdx[4] = 61,
  .colorIdx[5] = 45,
  .colorIdx[6] = 31,
  .colorIdx[7] = 3,
  .colorIdx[8] = 35,
  .colorIdx[9] = 59,
  .colorIdx[10] = 63
};

// Initialize values
static int valueRead, valueWritten;

static void deinit();

static Window *s_main_window;

static Layer *s_circle_layer;
static int s_radius = 30;	// Set circle radius sizes

// Array of colors
// Side note: I did not type this all out
// I scrapped the values via python script
static uint8_t allColors[64] = {(uint8_t)0b11000000, (uint8_t)0b11000001, (uint8_t)0b11000010, (uint8_t)0b11000011, (uint8_t)0b11000100, (uint8_t)0b11000101, (uint8_t)0b11000110, (uint8_t)0b11000111, (uint8_t)0b11001000, (uint8_t)0b11001001, (uint8_t)0b11001010, (uint8_t)0b11001011, (uint8_t)0b11001100, (uint8_t)0b11001101, (uint8_t)0b11001110, (uint8_t)0b11001111, (uint8_t)0b11010000, (uint8_t)0b11010001, (uint8_t)0b11010010, (uint8_t)0b11010011, (uint8_t)0b11010100, (uint8_t)0b11010101, (uint8_t)0b11010110, (uint8_t)0b11010111, (uint8_t)0b11011000, (uint8_t)0b11011001, (uint8_t)0b11011010, (uint8_t)0b11011011, (uint8_t)0b11011100, (uint8_t)0b11011101, (uint8_t)0b11011110, (uint8_t)0b11011111, (uint8_t)0b11100000, (uint8_t)0b11100001, (uint8_t)0b11100010, (uint8_t)0b11100011, (uint8_t)0b11100100, (uint8_t)0b11100101, (uint8_t)0b11100110, (uint8_t)0b11100111, (uint8_t)0b11101000, (uint8_t)0b11101001, (uint8_t)0b11101010, (uint8_t)0b11101011, (uint8_t)0b11101100, (uint8_t)0b11101101, (uint8_t)0b11101110, (uint8_t)0b11101111, (uint8_t)0b11110000, (uint8_t)0b11110001, (uint8_t)0b11110010, (uint8_t)0b11110011, (uint8_t)0b11110100, (uint8_t)0b11110101, (uint8_t)0b11110110, (uint8_t)0b11110111, (uint8_t)0b11111000, (uint8_t)0b11111001, (uint8_t)0b11111010, (uint8_t)0b11111011, (uint8_t)0b11111100, (uint8_t)0b11111101, (uint8_t)0b11111110, (uint8_t)0b11111111}; 

// This returns a pointer referring to current time
// WARNING: Caller must remember to free allocated buffer memory returned from this function
char * getTimeBuffer() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  char *buffer;
  buffer = (char*)malloc( 5 );

// Exits if there is a memory error
  if ( buffer == 0 ) {
    deinit();
  }

//  Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
 }
  return buffer;
}

// Updates the graphic drawing on the screen
static void update_proc(Layer *layer, GContext *ctx) {

  graphics_context_set_antialiased(ctx, S_TRUE);

  // Circle layer points
  int16_t x;
  int16_t y;
  
  char *buf = getTimeBuffer();
  char buf1[2];
    
  
  // circle
  for (int16_t i = 0; i < 4; i++){
    if (i<2) {
      y = 48;
    } else {
      y = 120;
    }
    x = 36 + i%2*72;
    GPoint s_center = {x,y};                

    int j;
    if( i<=1 ) {
      j = i;
    } else {
      j = i +1;
    }
    buf1[0]=buf[j]; 
    buf1[1]='\0';
    int idx = atoi( buf1 );

    // Fill circle with color
    graphics_context_set_fill_color(ctx, (GColor8)allColors[settings.colorIdx[idx]]);
    graphics_fill_circle(ctx, s_center, s_radius);
}
  //Remember to FREE memory
  free( buf );
}

static void main_window_load(Window *window) {
  
  // Create circle layer
  s_circle_layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(s_circle_layer, update_proc);
  layer_add_child(window_get_root_layer(window), s_circle_layer);
  
}

static void main_window_unload(Window *window) {
    // Destroy Layer
    layer_destroy(s_circle_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_main_window));
}

void bluetooth_connection_callback (bool connected) {
    if (!connected) {
    //&& settings.BluetoothVibe) {
		// vibe!
		  vibes_long_pulse();
    }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  
  APP_LOG(APP_LOG_LEVEL_INFO, "inbox callback success!");
  
  // Read first item
  Tuple *t = dict_read_first(iterator);
 
  // For all items
  while(t != NULL) {
    
    // Get string
    char colorString[strlen(t->value->cstring)+1];
    strcpy(colorString, t->value->cstring);
    const char s[2] = ",";
    char *token;
    int idx;
    
    /* get the first token */
    token = myStrtok(colorString, s);
    
    /* walk through other tokens */
    int count = 0;
    while( token != NULL ) 
    {
      idx = atoi( token );
      settings.colorIdx[count++] = idx;
      token = myStrtok(NULL, s);
    }
 
      // Look for next item
      t = dict_read_next(iterator);
    }
    window_set_background_color(s_main_window, (GColor8)allColors[settings.colorIdx[10]]);
    layer_mark_dirty(window_get_root_layer(s_main_window));
}
 
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}
 
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}
 
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void loadPersistentSettings() {  
  valueRead = persist_read_data(KEY_COLORS, &settings, sizeof(settings));
}

static void savePersistentSettings() {
  valueWritten = persist_write_data(KEY_COLORS, &settings, sizeof(settings));
}

static void init() {
  
  loadPersistentSettings();
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set color background
  window_set_background_color(s_main_window, (GColor8)allColors[settings.colorIdx[10]]);


  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Subscribe to Bluetooth Service
  bluetooth_connection_service_subscribe(bluetooth_connection_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
}

static void deinit() {
    // Destroy Window
    savePersistentSettings();
    window_destroy(s_main_window);
    bluetooth_connection_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
