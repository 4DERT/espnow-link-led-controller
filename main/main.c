#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "mode.h"
#include "nvs_manager.h"
#include "link_handler.h"

static const char *TAG = "main";

int app_main() {
  nvsm_init();

  bool is_mode_changed = mode_init();
  mode_e current_mode = mode_get();
  lh_init(current_mode, (is_mode_changed || true /*todo: check if button is pressed*/));

  while (1) {
    ESP_LOGI(TAG, "Hello World!");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

