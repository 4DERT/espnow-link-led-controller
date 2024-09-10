#include "encoder_rgbw_mode.h"
#include "led_controller.h"
#include "link_handler.h"

#define RGBW_BRIGHTNESS_FACTOR 5

static bool is_on = false;    // Keep track of the on/off state
static size_t color_mode = 0; // 0: All, 1: Red, 2: Green, 3: Blue, 4: White
static uint8_t current_brightness = 100; // Default brightness is 100%

// Single click - toggle the RGBW controller on/off
void encoder_rgbw_on_click() {
  if (is_on) {
    lc_off(LC_CH_ALL, true); // Turn off all channels
  } else {

    lc_set_rgbw_brightness(
        current_brightness); // Set brightness to previous level

    lc_on(LC_CH_ALL, true); // Turn on all channels
  }
  is_on = !is_on;
}

// Rotate left (CCW) - decrease brightness or color intensity for the selected
// mode
void encoder_rgbw_on_ccw() {
  if (is_on) {
    if (color_mode == 0) {
      // Decrease brightness for all channels (global brightness control)
      if (current_brightness > RGBW_BRIGHTNESS_FACTOR) {
        current_brightness -= RGBW_BRIGHTNESS_FACTOR;
      } else {
        current_brightness = 0;
      }
      lc_set_rgbw_brightness(current_brightness);
    } else {
      // Decrease intensity for the selected color
      const lc_channel_t *status = lc_get_status();
      int new_color_value =
          status[color_mode - 1].color_value - RGBW_BRIGHTNESS_FACTOR;
      if (new_color_value < LC_MIN_RGBW)
        new_color_value = LC_MIN_RGBW;

      switch (color_mode) {
      case 1:
        lc_set_rgbw(new_color_value, status[1].color_value,
                    status[2].color_value, status[3].color_value);
        break; // Red
      case 2:
        lc_set_rgbw(status[0].color_value, new_color_value,
                    status[2].color_value, status[3].color_value);
        break; // Green
      case 3:
        lc_set_rgbw(status[0].color_value, status[1].color_value,
                    new_color_value, status[3].color_value);
        break; // Blue
      case 4:
        lc_set_rgbw(status[0].color_value, status[1].color_value,
                    status[2].color_value, new_color_value);
        break; // White
      }
    }

    lc_on(LC_CH_RGBW_MAX, false);
  }
}

// Rotate right (CW) - increase brightness or color intensity for the selected
// mode
void encoder_rgbw_on_cw() {
  if (is_on) {
    if (color_mode == 0) {
      // Increase brightness for all channels (global brightness control)
      if (current_brightness < LC_MAX_BRIGHTNESS - RGBW_BRIGHTNESS_FACTOR) {
        current_brightness += RGBW_BRIGHTNESS_FACTOR;
      } else {
        current_brightness = LC_MAX_BRIGHTNESS;
      }
      lc_set_rgbw_brightness(current_brightness);
    } else {
      // Increase intensity for the selected color
      const lc_channel_t *status = lc_get_status();
      int new_color_value =
          status[color_mode - 1].color_value + RGBW_BRIGHTNESS_FACTOR;
      if (new_color_value > LC_MAX_RGBW)
        new_color_value = LC_MAX_RGBW;

      switch (color_mode) {
      case 1:
        lc_set_rgbw(new_color_value, status[1].color_value,
                    status[2].color_value, status[3].color_value);
        break; // Red
      case 2:
        lc_set_rgbw(status[0].color_value, new_color_value,
                    status[2].color_value, status[3].color_value);
        break; // Green
      case 3:
        lc_set_rgbw(status[0].color_value, status[1].color_value,
                    new_color_value, status[3].color_value);
        break; // Blue
      case 4:
        lc_set_rgbw(status[0].color_value, status[1].color_value,
                    status[2].color_value, new_color_value);
        break; // White
      }
    }

    lc_on(LC_CH_RGBW_MAX, false);
  }
}

// Double click - cycle between color modes (Red, Green, Blue, White, All)
void encoder_rgbw_on_double_click() {
  if (is_on) {
    color_mode = (color_mode + 1) %
                 5; // Cycle through 0: All, 1: Red, 2: Green, 3: Blue, 4: White
  }
}

// Long press - reset colors to default (white light)
void encoder_rgbw_on_long_press() {
  if (is_on) {
    lc_set_rgbw(255, 255, 255, 255); // Reset to white light
    current_brightness = 100;        // Reset brightness to 100%
    lc_set_rgbw_brightness(current_brightness);
    lc_on(LC_CH_RGBW_MAX, true);
  }
}
