#include "pebble.h"
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h> 

#include "ui.h"

/***** Useful Macros *****/
	
#define MAX(a, b) (( a > b)? a : b)

/***** Variables *****/	

// Texts

static TextLayer *bottleTextLayer;
static char timeTextUp[] = "00:00";	// Used by the system later
static TextLayer *bottleSinceTextLayer;
static char timeSinceTextUp[] = "(99 minutes ago)";	// Used by the system later

static TextLayer *diaperTextLayer;
static char timeTextMiddle[] = "00:00";	// Used by the system later
static TextLayer *diaperSinceTextLayer;
static char timeSinceTextMiddle[] = "(99 minutes ago)";	// Used by the system later

static TextLayer *moonTextLayer;
static char timeTextDown[14] = "";	// Used by the system later
static TextLayer *moonSinceTextLayer;
static char timeSinceTextDown[] = "(99 minutes ago)";	// Used by the system later

// Action Bar

static GBitmap *actionBottle;
static GBitmap *actionDiaper;
static GBitmap *actionMoon;

static ActionBarLayer *actionBar;

// Status bar
#ifdef PBL_SDK_3
static StatusBarLayer *s_status_bar;
#endif

// Data

static int sleeping = 0;
static time_t bottleStart = 0;
static time_t diaperStart = 0;
static time_t sleepStart = 0;
static time_t sleepEnd = 0;


/***** Util *****/

static void setTimeText(time_t timestamp, char *text, TextLayer *textLayer) {
	if (timestamp == 0) {
		text[0] = '\0';
	} else {
		struct tm *time = localtime(&timestamp);
		strftime(text, sizeof(timeTextUp), (clock_is_24h_style()? "%H:%M" : "%I:%M"), time);
	}
	text_layer_set_text(textLayer, text);
}

static void setTimeSinceText(time_t timestamp, char *text, TextLayer *textLayer) {
	
	if (timestamp > 0) {
		time_t now = time(NULL);
		time_t elapsed = now - timestamp;

		int minutes = elapsed / 60 % 60;
		int hours = elapsed / 60 / 60;
    
		if (elapsed < 60) {
			strcpy(text, "(just now)");
		} else if (elapsed < 3600) { // minutes
			snprintf(text, sizeof(timeSinceTextUp), "(%d min ago)", minutes);
		} else { // hours
			snprintf(text, sizeof(timeSinceTextUp), "(%dh%dm ago)", hours, minutes);
		}
	} else {
		text[0] = '\0';
	}
	
	// strftime(text, sizeof(timeTextUp), (clock_is_24h_style()? "%H:%M" : "%I:%M"), time);
	text_layer_set_text(textLayer, text);
}


static void setTimeRangeText(time_t startTimestamp, time_t endTimestamp, char *text, TextLayer *textLayer) {
	char sleepStartStr[] = "00:00";
	char sleepEndStr[] = "00:00";
	
	if (startTimestamp == 0 && endTimestamp == 0) {
		text[0] = '\0';
	} else {
		struct tm *time = localtime(&startTimestamp);

		strftime(sleepStartStr, sizeof(sleepStartStr), (clock_is_24h_style()? "%H:%M" : "%I:%M"), time);


		if (endTimestamp != 0) {
			time = localtime(&endTimestamp);
			strftime(sleepEndStr, sizeof(sleepEndStr), (clock_is_24h_style()? "%H:%M" : "%I:%M"), time);
		} else {
			strcpy(sleepEndStr, "...");
		}

		strncpy(text, sleepStartStr, sizeof(sleepStartStr));
		strncat(text, " - ", 4);
		strncat(text, sleepEndStr, sizeof(sleepEndStr));
	}
	
	text_layer_set_text(textLayer, text);
}

/***** App *****/

void ui_init(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
  
#ifdef PBL_SDK_3
  // Set up the status bar last to ensure it is on top of other Layers
  s_status_bar = status_bar_layer_create();
  layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));
#endif

	GRect bounds = layer_get_bounds(window_layer);
  int statusSize = 16;
	bounds.size.h -= statusSize;

	// Text layers
  int spacerSize = 5;
	int sectionSize = (bounds.size.h - (spacerSize * 3) - statusSize) / 3;
  int topOffset = statusSize;
  int middleOffset = topOffset + sectionSize + spacerSize;
  int bottomOffset = middleOffset + sectionSize + spacerSize;
  
  int width = bounds.size.w - ACTION_BAR_WIDTH;
  int timeH = 24;
  int sinceH = 20;
  
  // Bottle text
  // time
  bottleTextLayer = text_layer_create((GRect){ .origin = {0, topOffset }, .size = {width, timeH} });
	text_layer_set_text_alignment(bottleTextLayer, GTextAlignmentCenter);
	text_layer_set_text(bottleTextLayer, "");
	text_layer_set_font(bottleTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(bottleTextLayer));
  // since
  bottleSinceTextLayer = text_layer_create((GRect){ .origin = {0, topOffset + timeH }, .size = {width, sinceH} });
	text_layer_set_text_alignment(bottleSinceTextLayer, GTextAlignmentCenter);
	text_layer_set_text(bottleSinceTextLayer, "");
	text_layer_set_font(bottleSinceTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(bottleSinceTextLayer));
	
  // Diaper text
  // time
	diaperSinceTextLayer = text_layer_create((GRect){ .origin = {0, middleOffset + timeH }, .size = {width, sinceH} });
	text_layer_set_font(diaperSinceTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(diaperSinceTextLayer, GTextAlignmentCenter);
	text_layer_set_text(diaperSinceTextLayer, "");
	layer_add_child(window_layer, text_layer_get_layer(diaperSinceTextLayer));
  // since
	diaperTextLayer = text_layer_create((GRect){ .origin = {0, middleOffset }, .size = {width, timeH} });
	text_layer_set_font(diaperTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(diaperTextLayer, GTextAlignmentCenter);
	text_layer_set_text(diaperTextLayer, "");
	layer_add_child(window_layer, text_layer_get_layer(diaperTextLayer));
	  
  // Sleep
  // time
	moonTextLayer = text_layer_create((GRect){ .origin = {0, bottomOffset }, .size = {width, timeH} });
	text_layer_set_font(moonTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(moonTextLayer, GTextAlignmentCenter);
	text_layer_set_text(moonTextLayer, "00:00");
	layer_add_child(window_layer, text_layer_get_layer(moonTextLayer));
	// since
	moonSinceTextLayer = text_layer_create((GRect){ .origin = {0, bottomOffset + timeH }, .size = {width, sinceH} });
	text_layer_set_font(moonSinceTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(moonSinceTextLayer, GTextAlignmentCenter);
	text_layer_set_text(moonSinceTextLayer, "(---)");
	layer_add_child(window_layer, text_layer_get_layer(moonSinceTextLayer));
  
	// Time values initialization
	if (persist_exists(PERSIST_BOTTLE)) {
		bottleStart = persist_read_int(PERSIST_BOTTLE);
		setTimeText(bottleStart, timeTextUp, bottleTextLayer);
		setTimeSinceText(bottleStart, timeSinceTextUp, bottleSinceTextLayer);
	}
	
	if (persist_exists(PERSIST_DIAPER)) {
		diaperStart = persist_read_int(PERSIST_DIAPER);
		setTimeText(diaperStart, timeTextMiddle, diaperTextLayer);
		setTimeSinceText(diaperStart, timeSinceTextMiddle, diaperSinceTextLayer);
	}
	
	if (persist_exists(PERSIST_MOON_START)) {
		sleepStart = persist_read_int(PERSIST_MOON_START);
		
		sleepEnd = persist_exists(PERSIST_MOON_END)? persist_read_int(PERSIST_MOON_END) : 0;

		if (sleepEnd == 0 && sleepStart != 0) {
			sleeping = 1;
		}
		
		setTimeRangeText(sleepStart, sleepEnd, timeTextDown, moonTextLayer);
		setTimeSinceText(MAX(sleepStart, sleepEnd), timeSinceTextDown, moonSinceTextLayer);
	}
	
	// Action Bar
	
	// Initialize the action bar:
	actionBar = action_bar_layer_create();
	// Associate the action bar with the window:
	action_bar_layer_add_to_window(actionBar, window);
	// Set the click config provider:
	//action_bar_layer_set_click_config_provider(actionBar, (ClickConfigProvider) config_provider);
	
	// Set the icons:
	
	actionBottle = gbitmap_create_with_resource(RESOURCE_ID_ACTION_BOTTLE);
	actionDiaper = gbitmap_create_with_resource(RESOURCE_ID_ACTION_DIAPER);
	actionMoon = gbitmap_create_with_resource(RESOURCE_ID_ACTION_MOON);
	
	action_bar_layer_set_icon(actionBar, BUTTON_ID_UP, actionBottle);
	action_bar_layer_set_icon(actionBar, BUTTON_ID_SELECT, actionDiaper);
	action_bar_layer_set_icon(actionBar, BUTTON_ID_DOWN, actionMoon);
}

void ui_deinit() {
	text_layer_destroy(bottleTextLayer);
	text_layer_destroy(diaperTextLayer);
	text_layer_destroy(moonTextLayer);
	
	text_layer_destroy(bottleSinceTextLayer);
	text_layer_destroy(diaperSinceTextLayer);
	text_layer_destroy(moonSinceTextLayer);
	
	action_bar_layer_destroy(actionBar);

	gbitmap_destroy(actionBottle);
	gbitmap_destroy(actionDiaper);
	gbitmap_destroy(actionMoon);
}


void set_bottle_time(time_t timestamp) {
  bottleStart = timestamp;
  persist_write_int(PERSIST_BOTTLE, timestamp);
  setTimeSinceText(timestamp, timeSinceTextUp, bottleSinceTextLayer);
  setTimeText(timestamp, timeTextUp, bottleTextLayer);
}

void set_diaper_time(time_t timestamp) {
  diaperStart = timestamp;
  persist_write_int(PERSIST_DIAPER, timestamp);
  setTimeSinceText(timestamp, timeSinceTextMiddle, diaperSinceTextLayer);
  setTimeText(timestamp, timeTextMiddle, diaperTextLayer);
}

void set_sleep_time(time_t timestamp, bool asleep) {
  
}

void refreshTimes() {
	if (bottleStart != 0) setTimeSinceText(bottleStart, timeSinceTextUp, bottleSinceTextLayer);
	if (diaperStart != 0) setTimeSinceText(diaperStart, timeSinceTextMiddle, diaperSinceTextLayer);
	if (sleepStart != 0 || sleepEnd != 0) setTimeSinceText(MAX(sleepStart, sleepEnd), timeSinceTextDown, moonSinceTextLayer);
}
