#pragma once

void refreshTimes();
void set_bottle_time(time_t timestamp);
void set_diaper_time(time_t timestamp);
void set_sleep_time(time_t timestamp, bool asleep);
  
void ui_update();
void ui_init(Window *window);
void ui_deinit();

	
/***** Persist Keys *****/
#define PERSIST_BOTTLE 1
#define PERSIST_DIAPER 2
#define PERSIST_MOON_START 3
#define PERSIST_MOON_END 4

/*
void update_times(uint32_t asleep, uint32_t lastSleeping, uint32_t lastBottle, 
                  uint32_t lastNursing, uint32_t lastDiaper, uint32_t lastPumping);
*/