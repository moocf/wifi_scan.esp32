#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "macros.h"


static const char *TAG = "wifi_scan";


static esp_err_t nvs_init() {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ERET( nvs_flash_erase() );
    ERET( nvs_flash_init() );
  }
  ERET( ret );
  return ESP_OK;
}


static void on(void* arg, esp_event_base_t base, int32_t id, void* data) {
  if (base == WIFI_EVENT && id == WIFI_EVENT_SCAN_DONE) {
    static uint16_t count = 32;
    static wifi_ap_record_t records[32];
    printf("scan done\n");
    ERETV( esp_wifi_scan_get_ap_records(&count, records) );
    printf("got ap records\n");
    for(int i=0; i<count; i++) {
      printf("%d. %s : %d\n", i, records[i].ssid, records[i].rssi);
    }
  }
  if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
    esp_wifi_connect();
  } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) data;
    ESP_LOGI(TAG, "got ip: %s", ip4addr_ntoa(&event->ip_info.ip));
  }
}


/* Initialize Wi-Fi as sta and set scan method */
static esp_err_t wifi_scan() {
  tcpip_adapter_init();
  ERET( esp_event_loop_create_default() );

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ERET( esp_wifi_init(&cfg) );

  ERET( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &on, NULL) );
  ERET( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on, NULL) );
  wifi_config_t wifi_config = {.sta = {
    .ssid = "",
    .password = "",
    .scan_method = WIFI_ALL_CHANNEL_SCAN,
    .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
    .threshold.rssi = -127,
    .threshold.authmode = WIFI_AUTH_OPEN,
  }};
  ERET( esp_wifi_set_mode(WIFI_MODE_STA) );
  ERET( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
  printf("Jigglypuff\n");
  ERET( esp_wifi_start() );
  printf("Jiggly\n");
  wifi_scan_config_t sconfig = {
    .ssid = NULL,
    .bssid = NULL,
    .channel = 0,
    .show_hidden = 1,
    .scan_type = WIFI_SCAN_TYPE_ACTIVE,
    .scan_time.active.min = 120,
    .scan_time.active.max = 120
  };
  ERET( esp_wifi_scan_start(&sconfig, 0) );
  return ESP_OK;
}


void app_main() {
  ESP_ERROR_CHECK( nvs_init() );
  ESP_ERROR_CHECK( wifi_scan() );
}
