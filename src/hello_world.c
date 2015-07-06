#include <pebble.h>
#include "ui.h"
  
void init();
void deinit();

int main(void) {
	ui_init();
	app_event_loop();
	ui_deinit();
}



/*
#include <pebble.h>
#include "connect.h"
  
static Window *s_main_window;
static TextLayer *s_time_layer;

void init();
void deinit();
void main_window_load(Window *window);
void main_window_unload(Window *window);
void tick_handler(struct tm *tick_time, TimeUnits units_changed);
void update_time();

int main(void) {
	init();
	app_event_loop();
	deinit();
}

void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();
}

void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
}

void main_window_load(Window *window) {
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00:00");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Hook up data connection
  connect_init();
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  connect_deinit();
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00:00"), "%H:%M:%S", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00:00"), "%I:%M:%S", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}
*/