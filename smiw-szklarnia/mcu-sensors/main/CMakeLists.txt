idf_component_register(SRCS "mcu-sensors.c"
                    INCLUDE_DIRS "."
                    REQUIRES freertos esp_adc onewire_bus ds18b20)
