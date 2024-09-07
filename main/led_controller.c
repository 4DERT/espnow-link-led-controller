#include "led_controller.h"

#include <stdbool.h>

#include "freertos/FreeRTOS.h"

#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

#define LEDC_LS_TIMER LEDC_TIMER_1
#define LEDC_LS_MODE LEDC_LOW_SPEED_MODE
#define LEDC_LS_FREQ CONFIG_LEDC_FREQ
#define LEDC_LS_CH0_GPIO CONFIG_LEDC_CH_0_GPIO
#define LEDC_LS_CH0_CHANNEL LEDC_CHANNEL_0
#define LEDC_LS_CH1_GPIO CONFIG_LEDC_CH_1_GPIO
#define LEDC_LS_CH1_CHANNEL LEDC_CHANNEL_1
#define LEDC_LS_CH2_GPIO CONFIG_LEDC_CH_2_GPIO
#define LEDC_LS_CH2_CHANNEL LEDC_CHANNEL_2
#define LEDC_LS_CH3_GPIO CONFIG_LEDC_CH_3_GPIO
#define LEDC_LS_CH3_CHANNEL LEDC_CHANNEL_3

#define LEDC_CH_NUM (4)
#define LEDC_DUTY_RESOLUTION LC_DUTY_RESOLUTION
#define LEDC_MIN_DUTY LC_MIN_DUTY
#define LEDC_MAX_DUTY LC_MAX_DUTY
#define LEDC_FADE_TIME CONFIG_LEDC_FADE_TIME_MS

#define MAP_BRIGHTNESS_TO_DUTY(brightness)                                     \
  ((brightness) * ((1 << LEDC_DUTY_RESOLUTION) - 1) / 100)

#define MAP_COLOR_AND_BRIGHTNESS_TO_DUTY(color, brightness)                    \
  ((uint32_t)(((color) * (brightness) * LC_MAX_DUTY) /                         \
              (LC_MAX_RGBW * LC_MAX_BRIGHTNESS)))

static const char *TAG = "LED_controller";

lc_channel_t lc_channel_helper[LEDC_CH_NUM];

/*
 * Prepare and set configuration of timers
 * that will be used by LED Controller
 */
ledc_timer_config_t ledc_timer = {
    .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
    .freq_hz = LEDC_LS_FREQ,              // frequency of PWM signal
    .speed_mode = LEDC_LS_MODE,           // timer mode
    .timer_num = LEDC_LS_TIMER,           // timer index
    .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
};

/*
 * Prepare individual configuration
 * for each channel of LED Controller
 * by selecting:
 * - controller's channel number
 * - output duty cycle, set initially to 0
 * - GPIO number where LED is connected to
 * - speed mode, either high or low
 * - timer servicing selected channel
 *   Note: if different channels use one timer,
 *         then frequency and bit_num of these channels
 *         will be the same
 */
ledc_channel_config_t ledc_channel[LEDC_CH_NUM] = {
    {.channel = LEDC_LS_CH0_CHANNEL,
     .duty = 0,
     .gpio_num = LEDC_LS_CH0_GPIO,
     .speed_mode = LEDC_LS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_LS_TIMER,
     .flags.output_invert = 0},
    {.channel = LEDC_LS_CH1_CHANNEL,
     .duty = 0,
     .gpio_num = LEDC_LS_CH1_GPIO,
     .speed_mode = LEDC_LS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_LS_TIMER,
     .flags.output_invert = 0},
    {.channel = LEDC_LS_CH2_CHANNEL,
     .duty = 0,
     .gpio_num = LEDC_LS_CH2_GPIO,
     .speed_mode = LEDC_LS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_LS_TIMER,
     .flags.output_invert = 0},
    {.channel = LEDC_LS_CH3_CHANNEL,
     .duty = 0,
     .gpio_num = LEDC_LS_CH3_GPIO,
     .speed_mode = LEDC_LS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_LS_TIMER,
     .flags.output_invert = 0},
};

/*
 * This callback function will be called when fade operation has ended
 * Use callback only if you are aware it is being called inside an ISR
 * Otherwise, you can use a semaphore to unblock tasks
 */
static IRAM_ATTR bool on_ledc_fade_end_event(const ledc_cb_param_t *param,
                                             void *user_arg) {
  return true;
}

// Public

void lc_init() {
  int ch;

  // Set configuration of timer0 for high speed channels
  ledc_timer_config(&ledc_timer);

  // Set LED Controller with previously prepared configuration
  for (ch = 0; ch < LEDC_CH_NUM; ch++) {
    ledc_channel_config(&ledc_channel[ch]);
  }

  // Initialize fade service.
  ledc_fade_func_install(0);

  ledc_cbs_t callbacks = {.fade_cb = on_ledc_fade_end_event};

  for (ch = 0; ch < LEDC_CH_NUM; ch++) {
    ledc_cb_register(ledc_channel[ch].speed_mode, ledc_channel[ch].channel,
                     &callbacks, NULL);
  }

  // disable all leds
  for (ch = 0; ch < LEDC_CH_NUM; ch++) {
    ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel,
                  LEDC_MIN_DUTY);
    ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);

    // lc_channel_helper[ch].brightness = LC_MIN_BRIGHTNESS;
    lc_set_brightness(4, LC_MAX_BRIGHTNESS / 4);
  }
}

void lc_on(lc_channel_e ch, bool fade) {
  if (ch > LC_CH_ALL) {
    ESP_LOGW(TAG, "channel out of range");
    return;
  }

  if (ch == LC_CH_ALL) {
    for (int i = 0; i < LEDC_CH_NUM; i++) {
      lc_on(i, fade);
    }
    return;
  }

  ESP_LOGI(TAG, "Turning on channel %d", ch);

  if (fade) {
    ledc_set_fade_with_time(ledc_channel[ch].speed_mode,
                            ledc_channel[ch].channel,
                            lc_channel_helper[ch].duty, LEDC_FADE_TIME);
    ledc_fade_start(ledc_channel[ch].speed_mode, ledc_channel[ch].channel,
                    LEDC_FADE_NO_WAIT);
  } else {
    ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel,
                  lc_channel_helper[ch].duty);
    ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
  }

  lc_channel_helper[ch].is_on = true;
}

void lc_off(lc_channel_e ch, bool fade) {
  if (ch > LC_CH_ALL) {
    ESP_LOGW(TAG, "channel out of range");
    return;
  }

  if (ch == LC_CH_ALL) {
    for (int i = 0; i < LEDC_CH_NUM; i++) {
      lc_off(i, fade);
    }
    return;
  }

  ESP_LOGI(TAG, "Turning off channel %d", ch);

  if (fade) {
    ledc_set_fade_with_time(ledc_channel[ch].speed_mode,
                            ledc_channel[ch].channel, LEDC_MIN_DUTY,
                            LEDC_FADE_TIME);
    ledc_fade_start(ledc_channel[ch].speed_mode, ledc_channel[ch].channel,
                    LEDC_FADE_NO_WAIT);
  } else {
    ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel,
                  LEDC_MIN_DUTY);
    ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
  }

  lc_channel_helper[ch].is_on = false;
}

void lc_set_brightness(lc_channel_e ch, uint8_t brightness) {
  if (brightness < LC_MIN_BRIGHTNESS || brightness > LC_MAX_BRIGHTNESS) {
    ESP_LOGW(TAG, "Brightness out of range");
    return;
  }

  if (ch > LC_CH_ALL) {
    ESP_LOGW(TAG, "channel out of range");
    return;
  }

  if (ch == LC_CH_ALL) {
    for (int i = 0; i < LEDC_CH_NUM; i++) {
      lc_set_brightness(i, brightness);
    }
    return;
  }

  lc_channel_helper[ch].duty = MAP_BRIGHTNESS_TO_DUTY(brightness);
  lc_channel_helper[ch].brightness = brightness;

  ESP_LOGI(TAG, "Setting brightness of channel %d to %d%% (%lu)", ch,
           brightness, lc_channel_helper[ch].duty);
}

const lc_channel_t *lc_get_status() { return lc_channel_helper; }

void lc_set_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  lc_channel_helper[LC_CH_R].color_value = r;
  lc_channel_helper[LC_CH_R].duty = MAP_COLOR_AND_BRIGHTNESS_TO_DUTY(
      r, lc_channel_helper[LC_CH_R].brightness);
  ESP_LOGI(TAG, "Setting duty of channel %d to %lu", LC_CH_R,
           lc_channel_helper[LC_CH_R].duty);

  lc_channel_helper[LC_CH_G].color_value = g;
  lc_channel_helper[LC_CH_G].duty = MAP_COLOR_AND_BRIGHTNESS_TO_DUTY(
      g, lc_channel_helper[LC_CH_G].brightness);
  ESP_LOGI(TAG, "Setting duty of channel %d to %lu", LC_CH_G,
           lc_channel_helper[LC_CH_G].duty);

  lc_channel_helper[LC_CH_B].color_value = b;
  lc_channel_helper[LC_CH_B].duty = MAP_COLOR_AND_BRIGHTNESS_TO_DUTY(
      b, lc_channel_helper[LC_CH_B].brightness);
  ESP_LOGI(TAG, "Setting duty of channel %d to %lu", LC_CH_B,
           lc_channel_helper[LC_CH_B].duty);

  lc_channel_helper[LC_CH_W].color_value = w;
  lc_channel_helper[LC_CH_W].duty = MAP_COLOR_AND_BRIGHTNESS_TO_DUTY(
      w, lc_channel_helper[LC_CH_W].brightness);
  ESP_LOGI(TAG, "Setting duty of channel %d to %lu", LC_CH_W,
           lc_channel_helper[LC_CH_W].duty);
}

void lc_set_rgbw_brightness(uint8_t brightness) {
  if (brightness < LC_MIN_BRIGHTNESS || brightness > LC_MAX_BRIGHTNESS) {
    ESP_LOGW(TAG, "Brightness out of range");
    return;
  }

  for (int ch = 0; ch < LC_CH_ALL; ch++) {
    lc_channel_helper[ch].brightness = brightness;
    lc_channel_helper[ch].duty = MAP_COLOR_AND_BRIGHTNESS_TO_DUTY(
        lc_channel_helper[ch].color_value, brightness);
    ESP_LOGI(TAG, "Setting duty of channel %d to %lu", ch,
             lc_channel_helper[ch].duty);
  }
}