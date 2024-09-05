#include "encoder.h"

#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "encoder_ac_mode.h"
#include "encoder_ic_mode.h"
#include "encoder_rgbw_mode.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "iot_button.h"
#include "led_controller.h"
#include "mode.h"

#define ENCODER_WATCH_POINT_QUEUE_SIZE 10

static const char *TAG = "encoder";

static pcnt_unit_handle_t pcnt_unit = NULL;
static QueueHandle_t watch_point_queue;

static mode_e led_mode;

// button configuration
button_handle_t button;
button_config_t button_cfg = {
    .type = BUTTON_TYPE_GPIO,
    .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
    .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
    .gpio_button_config =
        {
            .gpio_num = CONFIG_ENCODER_BUTTON_GPIO,
            .active_level = CONFIG_ENCODER_BUTTON_GPIO_ACTIVE_LEVEL,
        },
};

// encoder events
static void on_encoder_ccw() {
  ESP_LOGI(TAG, "<");
  switch(led_mode) {
  case MODE_ALL_CHANNELS:
    encoder_ac_on_ccw();
    break;

  case MODE_INDIVIDUAL_CHANNELS:
    encoder_ic_on_ccw();
    break;

  case MODE_RGB:
    encoder_rgbw_on_ccw();
    break;

  default:
    break;
  }
}

static void on_encoder_cw() {
  ESP_LOGI(TAG, ">");
  switch(led_mode) {
  case MODE_ALL_CHANNELS:
    encoder_ac_on_cw();
    break;

  case MODE_INDIVIDUAL_CHANNELS:
    encoder_ic_on_cw();
    break;

  case MODE_RGB:
    encoder_rgbw_on_cw();
    break;

  default:
    break;
  }
}

static void on_encoder_button_single_click(void *arg, void *usr_data) {
  ESP_LOGI(TAG, "click");
  switch (led_mode) {
  case MODE_ALL_CHANNELS:
    encoder_ac_on_click();
    break;

  case MODE_INDIVIDUAL_CHANNELS:
    encoder_ic_on_click();
    break;

  case MODE_RGB:
    encoder_rgbw_on_click();
    break;

  default:
    break;
  }
}

// encoder task

void encoder_task(void *params) {
  int event_count = 0;
  while (1) {
    xQueueReceive(watch_point_queue, &event_count, portMAX_DELAY);

    if (event_count < 0) {
      // LEFT (ccw)
      on_encoder_ccw();
    } else {
      // RIGHT (cw)
      on_encoder_cw();
    }
  }
}

static bool pcnt_on_reach(pcnt_unit_handle_t unit,
                          const pcnt_watch_event_data_t *edata,
                          void *user_ctx) {
  BaseType_t high_task_wakeup;
  QueueHandle_t queue = (QueueHandle_t)user_ctx;

  // send event data to queue, from this interrupt callback
  xQueueSendFromISR(queue, &(edata->watch_point_value), &high_task_wakeup);
  pcnt_unit_clear_count(pcnt_unit);

  return (high_task_wakeup == pdTRUE);
}

void encoder_init(mode_e mode) {
  led_mode = mode;

  button = iot_button_create(&button_cfg);
  if (NULL == button) {
    ESP_LOGE(TAG, "Encoder button create failed");
  }
  iot_button_register_cb(button, BUTTON_SINGLE_CLICK,
                         on_encoder_button_single_click, NULL);

  ESP_LOGI(TAG, "install pcnt unit");
  pcnt_unit_config_t unit_config = {
      .high_limit = ENCODER_PULSES_PER_STEP * 2, // no matter in this example
      .low_limit = ENCODER_PULSES_PER_STEP * -2,
  };

  pcnt_new_unit(&unit_config, &pcnt_unit);

  ESP_LOGI(TAG, "set glitch filter");
  pcnt_glitch_filter_config_t filter_config = {
      .max_glitch_ns = ENCODER_GLITCH_FILTER_TIME_NS,
  };
  pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config);

  ESP_LOGI(TAG, "install pcnt channels");
  pcnt_chan_config_t chan_a_config = {
      .edge_gpio_num = ENCODER_GPIO_A,
      .level_gpio_num = ENCODER_GPIO_B,
  };
  pcnt_channel_handle_t pcnt_chan_a = NULL;
  pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a);
  pcnt_chan_config_t chan_b_config = {
      .edge_gpio_num = ENCODER_GPIO_B,
      .level_gpio_num = ENCODER_GPIO_A,
  };
  pcnt_channel_handle_t pcnt_chan_b = NULL;
  pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b);

  ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
  pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE,
                               PCNT_CHANNEL_EDGE_ACTION_INCREASE);
  pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
  pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE,
                               PCNT_CHANNEL_EDGE_ACTION_DECREASE);
  pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                PCNT_CHANNEL_LEVEL_ACTION_INVERSE);

  ESP_LOGI(TAG, "add watch points and register callbacks");
  pcnt_unit_add_watch_point(pcnt_unit, ENCODER_PULSES_PER_STEP * -1);
  pcnt_unit_add_watch_point(pcnt_unit, ENCODER_PULSES_PER_STEP);
  pcnt_event_callbacks_t cbs = {
      .on_reach = pcnt_on_reach,
  };
  watch_point_queue = xQueueCreate(ENCODER_WATCH_POINT_QUEUE_SIZE, sizeof(int));
  pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, watch_point_queue);

  pcnt_unit_enable(pcnt_unit);
  pcnt_unit_clear_count(pcnt_unit);
  pcnt_unit_start(pcnt_unit);

  xTaskCreate(encoder_task, "encoder_task", 4096, NULL, 1, NULL);
}