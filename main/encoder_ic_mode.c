#include "encoder_ic_mode.h"

#include "led_controller.h"
#include "link_handler.h"

#define E_IC_BRIGHTNESS_FACTOR 2

void encoder_ic_on_click() {

  const lc_channel_t *status = lc_get_status();

  bool are_all_channels_on = false;

  for (size_t ch = 0; ch < LC_CH_ALL; ch++) {
    if (status[ch].is_on) {
      are_all_channels_on = true;
    } else {
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

void encoder_ic_on_ccw() {

  const lc_channel_t *status = lc_get_status();
  for (size_t ch = 0; ch < LC_CH_ALL; ch++) {
    if (status[ch].is_on) {
      lc_set_brightness(ch, status[ch].brightness - E_IC_BRIGHTNESS_FACTOR);
      lc_on(ch, false);
    }
  }
}

void encoder_ic_on_cw() {

  const lc_channel_t *status = lc_get_status();
  for (size_t ch = 0; ch < LC_CH_ALL; ch++) {
    if (status[ch].is_on) {
      lc_set_brightness(ch, status[ch].brightness + E_IC_BRIGHTNESS_FACTOR);
      lc_on(ch, false);
    }
  }
}

void encoder_ic_on_double_click() {

}

void encoder_ic_on_long_press() {
  
}