#include "link_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"

#include "led_controller.h"
#include "link.h"

static const char *TAG = "link_handler";

static void on_link_command_all_channels(const char *cmd);
static void on_link_command_individual_channels(const char *cmd);
static void on_link_command_rgb_channels(const char *cmd);

static char *on_link_status_message_create_all_channels(void);
static char *on_link_status_message_create_individual_channels(void);
static char *on_link_status_message_create_rgb(void);

link_config_t link_config_all_channels = {
    .type = 3,
    .commands = {"ON", "OFF", "SET_BRIGHTNESS=*", "GET_STATUS"},
    .status_fmt = "{\"state\":\"%s\",\"brightness\":%u}",
    .user_command_parser_cb = on_link_command_all_channels,
    .user_status_msg_cb = on_link_status_message_create_all_channels,
};

link_config_t link_config_individual_channels = {
    .type = 4,
    .commands = {"ON=*", "OFF=*", "SET_BRIGHTNESS=*", "GET_STATUS"},
    .status_fmt = "[{\"state\":\"%s\",\"brightness\":%u},{\"state\":\"%s\","
                  "\"brightness\":%u},{\"state\":\"%s\",\"brightness\":%u},{"
                  "\"state\":\"%s\",\"brightness\":%u}]",
    .user_command_parser_cb = on_link_command_individual_channels,
    .user_status_msg_cb = on_link_status_message_create_individual_channels,
};

link_config_t link_config_rgb = {
    .type = 5,
    .commands = {"ON", "OFF", "SET_BRIGHTNESS=*", "GET_STATUS", "SET_RGB=*"},
    .status_fmt = "{\"state\":\"%s\",\"rgb\":[%u,%u,%u], \"brightness\": %u}",
    .user_command_parser_cb = on_link_command_rgb_channels,
    .user_status_msg_cb = on_link_status_message_create_rgb,
};

// Public

void lh_init(mode_e mode, bool force_pair) {
  switch (mode) {
  case MODE_ALL_CHANNELS:
    link_register(&link_config_all_channels);
    break;

  case MODE_INDIVIDUAL_CHANNELS:
    link_register(&link_config_individual_channels);
    break;

  case MODE_RGB:
    link_register(&link_config_rgb);
    break;

  default:
    break;
  }

  link_start(force_pair);
}

// Private

void on_link_command_all_channels(const char *cmd) {
  ESP_LOGI(TAG, "Received command: %s", cmd);

  if (strcmp(cmd, "ON") == 0) {
    lc_on(LC_CH_ALL);
  }

  else if (strcmp(cmd, "OFF") == 0) {
    lc_off(LC_CH_ALL);
  }

  else if (strncmp(cmd, "SET_BRIGHTNESS=", 15) == 0) {
    const char *value_str = cmd + 15;
    int value = atoi(value_str);
    lc_set_brightness(LC_CH_ALL, value);
    lc_on(LC_CH_ALL);
  }

  else if (strcmp(cmd, "GET_STATUS") == 0) {
  }

  link_send_status_msg();
}

void on_link_command_individual_channels(const char *cmd) {
  ESP_LOGI(TAG, "Received command: %s", cmd);

  if (strncmp(cmd, "ON=", 3) == 0) {
    const char *value_str = cmd + 3;
    int value = atoi(value_str);
    lc_on(value);
  }

  else if (strncmp(cmd, "OFF=", 4) == 0) {
    const char *value_str = cmd + 4;
    int value = atoi(value_str);
    lc_off(value);
  }

  else if (strncmp(cmd, "SET_BRIGHTNESS=", 15) == 0) {
    int channel, brightness;
    const char *value_str = cmd + 15;

    if (sscanf(value_str, "%d,%d", &channel, &brightness) == 2) {
      lc_set_brightness(channel, brightness);
      lc_on(channel);
    } else {
      ESP_LOGW(TAG, "Invalid command format.");
    }
  }

  else if (strcmp(cmd, "GET_STATUS") == 0) {
  }

  link_send_status_msg();
}

void on_link_command_rgb_channels(const char *cmd) {
  ESP_LOGI(TAG, "Received command: %s", cmd);

  if (strcmp(cmd, "ON") == 0) {
  }

  else if (strcmp(cmd, "OFF") == 0) {
  }

  else if (strncmp(cmd, "SET_BRIGHTNESS=", 15) == 0) {
    const char *value_str = cmd + 15;
    int value = atoi(value_str);

    if (value > 0 && value <= 100) {
    }
  }

  else if (strncmp(cmd, "SET_RGB=", 8) == 0) {
    int r, g, b;
    const char *value_str = cmd + 8;

    if (sscanf(value_str, "%d,%d,%d", &r, &g, &b) == 3) {
      if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255) {

      } else {
        ESP_LOGW(TAG, "Invalid RGB value.");
      }
    } else {
      ESP_LOGW(TAG, "Invalid command format.");
    }
  }

  else if (strcmp(cmd, "GET_STATUS") == 0) {
  }

  link_send_status_msg();
}

char *on_link_status_message_create_all_channels(void) {
  const lc_channel_t *lc_channel_arr = lc_get_status();

  char *status = link_generate_status_message(
      NULL, lc_channel_arr[LC_CH_0].is_on ? "ON" : "OFF",
      lc_channel_arr[LC_CH_0].brightness);
  return status;
}

char *on_link_status_message_create_individual_channels(void) {
  const lc_channel_t *lc_channel_arr = lc_get_status();

  char *status = link_generate_status_message(
      NULL, lc_channel_arr[LC_CH_0].is_on ? "ON" : "OFF",
      lc_channel_arr[LC_CH_0].brightness,
      lc_channel_arr[LC_CH_1].is_on ? "ON" : "OFF",
      lc_channel_arr[LC_CH_1].brightness,
      lc_channel_arr[LC_CH_2].is_on ? "ON" : "OFF",
      lc_channel_arr[LC_CH_2].brightness,
      lc_channel_arr[LC_CH_3].is_on ? "ON" : "OFF",
      lc_channel_arr[LC_CH_3].brightness);
  return status;
}

char *on_link_status_message_create_rgb(void) {
  char *msg = strdup("OK");
  return msg;
}