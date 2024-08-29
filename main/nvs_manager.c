#include "nvs_manager.h"

#include <stdint.h>

#include "nvs.h"
#include "nvs_flash.h"
#include "esp_err.h"

void nvsm_init() {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
}

esp_err_t nvsm_save_int32_to_nvs(const char *namespace, const char *key,
                                 int32_t value) {

  nvs_handle_t my_handle;
  esp_err_t err = nvs_open(namespace, NVS_READWRITE, &my_handle);
  if (err != ESP_OK)
    return err;

  err = nvs_set_i32(my_handle, key, value);
  if (err != ESP_OK)
    return err;

  err = nvs_commit(my_handle);
  nvs_close(my_handle);

  return err;
}

esp_err_t nvsm_read_int32_from_nvs(const char *namespace, const char *key,
                                   int32_t *value) {

  nvs_handle_t my_handle;
  esp_err_t err = nvs_open(namespace, NVS_READONLY, &my_handle);
  if (err != ESP_OK)
    return err;

  err = nvs_get_i32(my_handle, key, value);
  nvs_close(my_handle);

  return err;
}