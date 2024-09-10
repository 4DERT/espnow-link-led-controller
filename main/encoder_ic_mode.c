#include "encoder_ic_mode.h"
#include "led_controller.h"
#include "link_handler.h"

#define E_IC_BRIGHTNESS_FACTOR 2

static size_t current_channel = 0;
static bool all_channels_selected = true;

// Single encoder click - toggles the on/off state of the currently selected
// channel or all channels if in global mode
void encoder_ic_on_click() {
  const lc_channel_t *status = lc_get_status();

  if (all_channels_selected) {
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
  } else {
    if (status[current_channel].is_on) {
      lc_off(current_channel, true);
    } else {
      lc_on(current_channel, true);
    }
  }
}

// Rotate left (CCW) - decreases the brightness of the selected channel or all
// channels if in global mode
void encoder_ic_on_ccw() {
  const lc_channel_t *status = lc_get_status();

  if (all_channels_selected) {
    // Decrease the brightness of all channels
    for (size_t ch = 0; ch < LC_CH_ALL; ch++) {
      if (status[ch].is_on) {
        int new_brightness = status[ch].brightness - E_IC_BRIGHTNESS_FACTOR;
        if (new_brightness < 0)
          new_brightness = 0;
        lc_set_brightness(ch, new_brightness);
        lc_on(ch, false);
      }
    }
  } else {
    // Decrease the brightness of the currently selected channel
    if (status[current_channel].is_on) {
      int new_brightness =
          status[current_channel].brightness - E_IC_BRIGHTNESS_FACTOR;
      if (new_brightness < 0)
        new_brightness = 0;
      lc_set_brightness(current_channel, new_brightness);
      lc_on(current_channel, false);
    }
  }
}

// Rotate right (CW) - increases the brightness of the selected channel or all
// channels if in global mode
void encoder_ic_on_cw() {
  const lc_channel_t *status = lc_get_status();

  if (all_channels_selected) {
    // Increase the brightness of all channels
    for (size_t ch = 0; ch < LC_CH_ALL; ch++) {
      if (status[ch].is_on) {
        int new_brightness = status[ch].brightness + E_IC_BRIGHTNESS_FACTOR;
        if (new_brightness > 100)
          new_brightness = 100;
        lc_set_brightness(ch, new_brightness);
        lc_on(ch, false);
      }
    }
  } else {
    // Increase the brightness of the currently selected channel
    if (status[current_channel].is_on) {
      int new_brightness =
          status[current_channel].brightness + E_IC_BRIGHTNESS_FACTOR;
      if (new_brightness > 100)
        new_brightness = 100;
      lc_set_brightness(current_channel, new_brightness);
      lc_on(current_channel, false);
    }
  }
}

// Double click - switches the currently controlled channel only in individual
// mode
void encoder_ic_on_double_click() {
  if (!all_channels_selected) {
    current_channel = (current_channel + 1) % LC_CH_ALL;
  }
}

// Long press - toggles between global mode (all channels) and individual mode
void encoder_ic_on_long_press() {
  all_channels_selected = !all_channels_selected; // Toggle mode

  if (all_channels_selected) {
    // Global mode enabled
    lc_on(LC_CH_ALL, false); // Optionally turn on all channels if required
  }
}
