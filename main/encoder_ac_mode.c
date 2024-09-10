#include "encoder_ac_mode.h"
#include "led_controller.h"
#include "link_handler.h"

#define AC_BRIGHTNESS_FACTOR 2

// Single click - toggle all channels on/off
void encoder_ac_on_click() {
  const lc_channel_t *status = lc_get_status();

  bool are_all_channels_on = true;
  for (size_t ch = 0; ch < LC_CH_ALL; ch++) {
    if (!status[ch].is_on) {
      are_all_channels_on = false;
      break;
    }
  }

  if (are_all_channels_on) {
    lc_off(LC_CH_ALL, true);
  } else {
    lc_on(LC_CH_ALL, true);
  }
}

// Rotate left (CCW) - decrease brightness for all channels
void encoder_ac_on_ccw() {
  const lc_channel_t *status = lc_get_status();

  for (size_t ch = 0; ch < LC_CH_ALL; ch++) {
    if (status[ch].is_on) {
      int new_brightness = status[ch].brightness - AC_BRIGHTNESS_FACTOR;
      if (new_brightness < 0)
        new_brightness = 0;
      lc_set_brightness(ch, new_brightness);
      lc_on(ch, false);
    }
  }
}

// Rotate right (CW) - increase brightness for all channels
void encoder_ac_on_cw() {
  const lc_channel_t *status = lc_get_status();

  for (size_t ch = 0; ch < LC_CH_ALL; ch++) {
    if (status[ch].is_on) {
      int new_brightness = status[ch].brightness + AC_BRIGHTNESS_FACTOR;
      if (new_brightness > 100)
        new_brightness = 100;
      lc_set_brightness(ch, new_brightness);
      lc_on(ch, false);
    }
  }
}

// Double click - not used
void encoder_ac_on_double_click() {
  // No functionality for double click
}

// Long press - not used
void encoder_ac_on_long_press() {
  // No functionality for long press
}
