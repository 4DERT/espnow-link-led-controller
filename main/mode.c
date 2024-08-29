#include "mode.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include "nvs_manager.h"

static const char *TAG = "mode";

static mode_e mode;

#define MODE_NVS_NAMESPACE "MODE"
#define MODE_NVS_LAST_MODE "LAST_MODE"

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

bool mode_init() {
  gpio_set_direction(CONFIG_GPIO_JUMPER_C1, GPIO_MODE_INPUT);
  gpio_set_direction(CONFIG_GPIO_JUMPER_C2, GPIO_MODE_INPUT);
  gpio_pullup_en(CONFIG_GPIO_JUMPER_C1);
  gpio_pullup_en(CONFIG_GPIO_JUMPER_C2);
  mode = specify_mode();

  ESP_LOGI(TAG, "The LED controller is configured in mode: %d", mode);

  int32_t last_mode;
  nvsm_read_int32_from_nvs(MODE_NVS_NAMESPACE, MODE_NVS_LAST_MODE, &last_mode);

  if (last_mode != mode) {
    ESP_LOGI(TAG, "The mode has changed since the last startup!");
    nvsm_save_int32_to_nvs(MODE_NVS_NAMESPACE, MODE_NVS_LAST_MODE,
                           (int32_t)mode);
    return true;
  }

  return false;
}

mode_e mode_get() { return mode; }