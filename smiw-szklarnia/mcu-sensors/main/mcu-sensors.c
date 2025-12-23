#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ds18b20.h"
#include "onewire_bus.h"
#include "esp_log.h"

static const char *TAG = "SENSOR";

#define ONEWIRE_BUS_GPIO    1


void app_main(void)
{
    onewire_bus_handle_t bus = NULL;
    onewire_bus_config_t bus_config = {
        .bus_gpio_num = ONEWIRE_BUS_GPIO,
    };

    onewire_bus_rmt_config_t rmt_config = {
        .max_rx_bytes = 20,
    };

    ESP_ERROR_CHECK(onewire_new_bus_rmt(&bus_config, &rmt_config, &bus));
    ESP_LOGI(TAG, "Szyna 1wire jest na %d", ONEWIRE_BUS_GPIO);

    onewire_device_iter_handle_t iter = NULL;
    onewire_device_t next_device;
    esp_err_t search_result = ESP_OK;
    ESP_ERROR_CHECK(onewire_new_device_iter(bus, &iter));
    search_result = onewire_device_iter_get_next(iter, &next_device);
    if(search_result != ESP_OK){
        ESP_LOGE(TAG, "Nie znaleziono żadnego urządzenia na szynie!");
        return;
    }
    
    ds18b20_device_handle_t ds18b20 = NULL;
    ds18b20_config_t ds_config = {}; 

    if (ds18b20_new_device_from_enumeration(&next_device, &ds_config, &ds18b20) == ESP_OK) {
        ESP_LOGI(TAG, "Znaleziono czujnik DS18B20");
    } else {
        ESP_LOGE(TAG, "Znalezione urządzenie to nie DS18B20");
        return;
    }

    ESP_ERROR_CHECK(onewire_del_device_iter(iter));

    float temperature;
    while (1) {
        ESP_ERROR_CHECK(ds18b20_trigger_temperature_conversion(ds18b20));
        
        vTaskDelay(pdMS_TO_TICKS(800));

        ESP_ERROR_CHECK(ds18b20_get_temperature(ds18b20, &temperature));
        
        ESP_LOGI(TAG, "Temperatura: %.2f °C", temperature);
        
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}