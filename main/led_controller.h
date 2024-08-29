#ifndef LED_CONTROLLER_H_
#define LED_CONTROLLER_H_

#include <stdint.h>

#include "driver/ledc.h"

#define LC_DUTY_RESOLUTION LEDC_TIMER_13_BIT
#define LC_MIN_DUTY 0
#define LC_MAX_DUTY (1 << LC_DUTY_RESOLUTION)
#define LC_MIN_BRIGHTNESS 1
#define LC_MAX_BRIGHTNESS 100

typedef enum { LC_CH_0, LC_CH_1, LC_CH_2, LC_CH_3, LC_CH_ALL } lc_channel_e;

typedef struct {
  bool is_on;
  uint8_t brightness;
  uint32_t duty;
} lc_channel_t;

void lc_init();
void lc_on(lc_channel_e ch);
void lc_off(lc_channel_e ch);
void lc_set_brightness(lc_channel_e ch, uint8_t brightness);
const lc_channel_t *lc_get_status();

#endif // LED_CONTROLLER_H_