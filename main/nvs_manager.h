#ifndef NVS_MANAGER_H_
#define NVS_MANAGER_H_

#include <stdint.h>
#include "esp_err.h"

void nvsm_init();
esp_err_t nvsm_save_int32_to_nvs(const char *namespace, const char *key,
                                 int32_t value);

esp_err_t nvsm_read_int32_from_nvs(const char *namespace, const char *key,
                                   int32_t *value);

#endif // NVS_MANAGER_H_