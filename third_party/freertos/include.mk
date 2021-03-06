TARGET_FREERTOS_PATH = $(FREERTOS_MODULE_PATH)
INCLUDE_DIRS += $(FREERTOS_MODULE_PATH)/freertos/FreeRTOS/Source/include

# FreeRTOS port
ifneq ("$(PLATFORM_FREERTOS)","")
TARGET_FREERTOS_PORT_INCLUDE_PATH = $(FREERTOS_MODULE_PATH)/freertos/FreeRTOS/Source/portable/GCC/$(PLATFORM_FREERTOS)
endif

# FIXME
ifeq ($(SOFTDEVICE_PRESENT),y)
INCLUDE_DIRS += $(TARGET_NRF5_SDK_PATH)/nrf5_sdk/external/freertos/portable/GCC/nrf52
INCLUDE_DIRS += $(TARGET_NRF5_SDK_PATH)/nrf5_sdk/external/freertos/portable/CMSIS/nrf52
else
INCLUDE_DIRS += $(TARGET_FREERTOS_PORT_INCLUDE_PATH)
endif
