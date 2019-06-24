#include <esp_log.h>
#include <esp_wifi.h>
#include "macros.h"


static const char *TAG = "wifi_scan";


esp_err_t app_main() {
  tcpip_adapter_init();
  ERET( esp_event_loop_create_default() );
  printf("Jigglyfuff\n");
  return ESP_OK;
}
