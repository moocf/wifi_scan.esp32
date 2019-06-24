#include <string.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include "macros.h"


static const char *TAG = "wifi_scan";


esp_err_t nvs_init() {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ERET( nvs_flash_erase() );
    ERET( nvs_flash_init() );
  }
  return ret;
}


esp_err_t app_main() {
  ERET( nvs_init() );
  tcpip_adapter_init();
  ERET( esp_event_loop_create_default() );
  wifi_init_config_t iconfig = WIFI_INIT_CONFIG_DEFAULT();
  printf("esp_wifi_init()\n");
  ERET( esp_wifi_init(&iconfig) );
  printf("esp_wifi_set_mode()\n");
  ERET( esp_wifi_set_mode(WIFI_MODE_STA) );
  wifi_scan_config_t sconfig = {
    .ssid = NULL,
    .bssid = NULL,
    .channel = 0,
    .show_hidden = false,
    .scan_type = WIFI_SCAN_TYPE_ACTIVE,
    .scan_time.active.min = 120,
    .scan_time.active.max = 120
  };
  printf("esp_wifi_scan_start()\n");
  ERET( esp_wifi_scan_start(&sconfig, true) );
  uint16_t count = 32;
  wifi_ap_record_t records[32];
  printf("esp_wifi_scan_get_ap_records()\n");
  ERET( esp_wifi_scan_get_ap_records(&count, records) );
  for (int i=0; i<count; i++) {
    printf("%s: %d\n", records[i].ssid, records[i].rssi);
  }
  return ESP_OK;
}
