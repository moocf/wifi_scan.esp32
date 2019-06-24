#pragma once

#define ERET(cmd) cmd;

/* #define ERET(cmd) \
  do { \
    esp_err_t ret = cmd; \
    if (ret == ESP_OK) break; \
    ESP_LOGE(TAG, "%s:%d %s", __FILE__ , __LINE__, esp_err_to_name(ret)); \
    return ret; \
  } while(0)
 */
