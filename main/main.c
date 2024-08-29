#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG = "main";

int app_main() {

  while (1) {
    ESP_LOGI(TAG, "Hello World!");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}