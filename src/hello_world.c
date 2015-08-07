#include "pebble.h"
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>

#include "connect.h"
#include "ui.h"
  
static Window *window;

/***** Tick Events *****/

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  refreshTimes();
}

/***** App *****/

static void window_load(Window *window) {
	ui_init(window);
 	connect_init();
}


static void window_unload(Window *window) {
  connect_deinit();
  ui_deinit();
}

static void init(void) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});
	//window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
	
	window_stack_push(window, true /* Animated */);
}

static void deinit(void) {
	window_destroy(window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}