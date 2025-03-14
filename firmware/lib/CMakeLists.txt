file(GLOB srcs ./*.c)
idf_component_register(SRCS ${srcs} INCLUDE_DIRS "." REQUIRES art32 naos driver esp_adc)
