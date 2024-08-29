#include "mode.h"

#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "mode";

static mode_e mode;

mode_e specify_mode() {
  bool c1 = (gpio_get_level(CONFIG_GPIO_JUMPER_C1) ==
             CONFIG_GPIO_JUMPER_C1_ACTIVE_STATE);
  bool c2 = (gpio_get_level(CONFIG_GPIO_JUMPER_C2) ==
             CONFIG_GPIO_JUMPER_C2_ACTIVE_STATE);

  if (c2) {
    return MODE_RGB;
  } else if (c1) {
    return MODE_ALL_CHANNELS;
  }
  return MODE_INDIVIDUAL_CHANNELS;
}

// Public

void mode_init() {
  gpio_set_direction(CONFIG_GPIO_JUMPER_C1, GPIO_MODE_INPUT);
  gpio_set_direction(CONFIG_GPIO_JUMPER_C2, GPIO_MODE_INPUT);
  mode = specify_mode();

  ESP_LOGI(TAG, "The LED controller is configured in mode: %d", mode);
}

mode_e mode_get() { return mode; }