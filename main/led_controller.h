#ifndef LED_CONTROLLER_H_
#define LED_CONTROLLER_H_

#include <stdint.h>

#include "driver/ledc.h"

#define LC_DUTY_RESOLUTION LEDC_TIMER_13_BIT
#define LC_MIN_DUTY 0
#define LC_MAX_DUTY (1 << LC_DUTY_RESOLUTION)
#define LC_MIN_BRIGHTNESS 1
#define LC_MAX_BRIGHTNESS 100
#define LC_MIN_RGBW 0
#define LC_MAX_RGBW 255

typedef enum { LC_CH_0, LC_CH_1, LC_CH_2, LC_CH_3, LC_CH_ALL } lc_channel_e;
typedef enum {
  LC_CH_R,
  LC_CH_G,
  LC_CH_B,
  LC_CH_W,
  LC_CH_RGBW_MAX
} lc_channel_rgbw_e;

typedef struct {
  bool is_on;
  uint8_t brightness;
  uint32_t duty;
  uint8_t color_value;
} lc_channel_t;

void lc_init();
const lc_channel_t *lc_get_status();
void lc_on(lc_channel_e ch);
void lc_off(lc_channel_e ch);
void lc_set_brightness(lc_channel_e ch, uint8_t brightness);
void lc_set_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void lc_set_rgbw_brightness(uint8_t brightness);

#endif // LED_CONTROLLER_H_