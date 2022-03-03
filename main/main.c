#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Startup...");
    ESP_LOGI(TAG, "Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "IDF version: %s", esp_get_idf_version());

    return;
}
