#ifndef LED_CONTROLLER_H_
#define LED_CONTROLLER_H_

#include <stdint.h>

typedef enum { LC_CH_0, LC_CH_1, LC_CH_2, LC_CH_3, LC_CH_ALL } lc_channel_e;

void lc_init();
void lc_on(lc_channel_e ch);
void lc_off(lc_channel_e ch);
void lc_set_brightness(lc_channel_e ch, uint8_t brightness);

#endif // LED_CONTROLLER_H_