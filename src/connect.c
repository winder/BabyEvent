#include <pebble.h>
#include <inttypes.h>

#include "connect.h"
#include "utils.h"
  
static AppSync sync;
static uint8_t sync_buffer[64];
static uint32_t data[] = {0,0,0,0,0,0};

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %s", translate_error(app_message_error));
}

uint32_t* getData() {return data;}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "sync tuple callback....");
  
  data[key] = new_tuple->value->uint32;
  
  switch (key) {
    case IS_SLEEPING:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Is sleeping: %"PRIu32, new_tuple->value->uint32);
        break;
    case LAST_SLEEPING:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Last feeding: %"PRIu32, new_tuple->value->uint32);
        break;
    case LAST_BOTTLE:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Last bottle: %"PRIu32, new_tuple->value->uint32);
        break;
    case LAST_NURSING:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Last nursing: %"PRIu32, new_tuple->value->uint32);
        break;
    case LAST_DIAPER:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Last diaper: %"PRIu32, new_tuple->value->uint32);
        break;
    case LAST_PUMPING:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Last pumping: %"PRIu32, new_tuple->value->uint32);
        break;
  }
}

void connect_init() {
  const int inbound_size = 64;
  const int outbound_size = 16;
  app_message_open(inbound_size, outbound_size);

  Tuplet initial_values[] = {
        TupletInteger(IS_SLEEPING, 0),
        TupletInteger(LAST_SLEEPING, 1),
        TupletInteger(LAST_BOTTLE, 2),
        TupletInteger(LAST_NURSING, 3),
        TupletInteger(LAST_DIAPER, 4),
        TupletInteger(LAST_PUMPING, 5),
    };
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, sync_error_callback, NULL);
}

void connect_deinit() {
    app_sync_deinit(&sync);
}