#ifndef AGGL_PLATFORM_H
#define AGGL_PLATFORM_H

// Platform abstraction layer for AGGL
// Supports both Arduino and ESP-IDF platforms

#ifdef ARDUINO
    // Arduino platform includes
    #include <Arduino.h>
    #define AGGL_PLATFORM_ARDUINO
    #define AGGL_LOG(...) Serial.printf(__VA_ARGS__)
    #define AGGL_LOG_ERROR(...) Serial.printf(__VA_ARGS__)
    #define AGGL_DELAY(ms) delay(ms)
    #define AGGL_MILLIS() millis()
    #define AGGL_MIN(a,b) min(a,b)
    #define AGGL_MAX(a,b) max(a,b)
    #define AGGL_ABS(a) abs(a)
#else
    // ESP-IDF platform includes
    #include <esp_log.h>
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
    #include <algorithm>
    #include <cmath>
    #include <cstring>
    #include <cstdint>

    #define AGGL_PLATFORM_ESP_IDF
    #define AGGL_LOG(...) ESP_LOGI("AGGL", __VA_ARGS__)
    #define AGGL_LOG_ERROR(...) ESP_LOGE("AGGL", __VA_ARGS__)
    #define AGGL_DELAY(ms) vTaskDelay(pdMS_TO_TICKS(ms))
    #define AGGL_MILLIS() (esp_timer_get_time() / 1000)
    #define AGGL_MIN(a,b) std::min(a,b)
    #define AGGL_MAX(a,b) std::max(a,b)
    #define AGGL_ABS(a) std::abs(a)

    // ESP-IDF specific types and functions
    using std::vector;
    using std::find;
    using std::swap;

#endif

// Common includes for both platforms
#include <cstdint>
#include <cstdlib>

// Memory management (works on both platforms)
#define AGGL_MALLOC(size) malloc(size)
#define AGGL_FREE(ptr) free(ptr)
#define AGGL_MEMCPY(dest, src, size) memcpy(dest, src, size)
#define AGGL_MEMSET(dest, val, size) memset(dest, val, size)

#endif // AGGL_PLATFORM_H