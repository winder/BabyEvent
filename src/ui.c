#include <pebble.h>
#include "ui.h"
#include "connect.h"
  /*
static Window *s_main_window;
//static TextLayer *s_time_layer;

static TextLayer *s_top_time_layer;
static TextLayer *s_mid_time_layer;
static TextLayer *s_bot_time_layer;




void main_window_load(Window *window);
void main_window_unload(Window *window);
void tick_handler(struct tm *tick_time, TimeUnits units_changed);
void update_time();

void ui_update() {
  
}

void ui_init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Register with TickTimerService
  //tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
}

void ui_deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

// ================== //
// Internal functions //
// ================== //





///
/// Below is crude but displays my data.
///

void main_window_load(Window *window) {
  // Create time TextLayers
  s_top_time_layer = text_layer_create(GRect(0, 0, 144, 50));
  text_layer_set_background_color(s_top_time_layer, GColorClear);
  text_layer_set_text_color(s_top_time_layer, GColorBlack);
  text_layer_set_text(s_top_time_layer, "00:00:00");

  s_mid_time_layer = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(s_mid_time_layer, GColorClear);
  text_layer_set_text_color(s_mid_time_layer, GColorBlack);
  text_layer_set_text(s_mid_time_layer, "00:00:00");

  s_bot_time_layer = text_layer_create(GRect(0, 110, 144, 50));
  text_layer_set_background_color(s_bot_time_layer, GColorClear);
  text_layer_set_text_color(s_bot_time_layer, GColorBlack);
  text_layer_set_text(s_bot_time_layer, "00:00:00");

  
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_top_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_top_time_layer, GTextAlignmentCenter);

  text_layer_set_font(s_mid_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_mid_time_layer, GTextAlignmentCenter);

  text_layer_set_font(s_bot_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_bot_time_layer, GTextAlignmentCenter);

  // Hook up data connection
  connect_init();
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_top_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_mid_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bot_time_layer));
}

void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_top_time_layer);
  text_layer_destroy(s_mid_time_layer);
  text_layer_destroy(s_bot_time_layer);
  
  connect_deinit();
}

void update_times(uint32_t asleep, uint32_t lastSleeping, uint32_t lastBottle, 
                  uint32_t lastNursing, uint32_t lastDiaper, uint32_t lastPumping) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating timer text");
  
  static char* buffer[] = {"10:00:00", "20:00:00", "30:00:00"};
  
  if (asleep == 0) {
    strncpy(buffer[0], "awake", sizeof("00:00:00"));
  } else {
    strncpy(buffer[0], "asleep", sizeof("00:00:00"));
  }
  
  time_t temp;
  temp = (time_t)lastSleeping;
  strftime(buffer[1], sizeof("00:00:00"), "%H:%M:%S", localtime(&temp));
  temp = (time_t)lastDiaper;
  strftime(buffer[2], sizeof("00:00:00"), "%H:%M:%S", localtime(&temp));

  // Display this time on the TextLayer
  text_layer_set_text(s_top_time_layer, buffer[0]);
  text_layer_set_text(s_mid_time_layer, buffer[1]);
  text_layer_set_text(s_bot_time_layer, buffer[2]);
}


void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  //update_time();
  uint32_t* data = getData();
  update_times(data[IS_SLEEPING],
               data[LAST_SLEEPING],
               data[LAST_BOTTLE],
               data[LAST_NURSING],
               data[LAST_DIAPER],
               data[LAST_PUMPING]);
}
*/