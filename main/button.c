#include "button.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void button_init() {
  gpio_reset_pin(BUTTON_GPIO);
  gpio_config_t button_cfg;
  button_cfg.pin_bit_mask = (1UL << BUTTON_GPIO);
  button_cfg.mode = GPIO_MODE_INPUT;
  button_cfg.pull_up_en =
      (BUTTON_ACTIVE_STATE) ? GPIO_PULLUP_DISABLE : GPIO_PULLUP_ENABLE;
  button_cfg.pull_down_en =
      (BUTTON_ACTIVE_STATE) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
  button_cfg.intr_type =
      (BUTTON_ACTIVE_STATE) ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE;
  gpio_config(&button_cfg);
}

bool button_is_pressed() {
    return (gpio_get_level(BUTTON_GPIO) == BUTTON_ACTIVE_STATE);
}
