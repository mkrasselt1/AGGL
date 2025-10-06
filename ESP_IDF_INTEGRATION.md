# AGGL - Cross-Platform Graphics Library

AGGL (Arduino Graphics Graphics Library) ist jetzt sowohl mit Arduino als auch mit ESP-IDF kompatibel. Die Bibliothek bietet eine einheitliche API für verschiedene Mikrocontroller-Plattformen.

## Unterstützte Plattformen

### Arduino
- Automatische Erkennung durch `ARDUINO` Define
- Verwendet Arduino Standardbibliotheken (`Arduino.h`, `Serial`, etc.)
- Kompatibel mit AVR, SAM, ESP32 (Arduino Framework)

### ESP-IDF
- Automatische Erkennung wenn `ARDUINO` nicht definiert ist
- Verwendet ESP-IDF APIs (`esp_log.h`, `freertos/FreeRTOS.h`, etc.)
- Native ESP32-Serie Unterstützung

## Plattform-spezifische Features

### Logging
```cpp
// Plattform-unabhängiges Logging
AGGL_LOG("Info message: %d", value);
AGGL_LOG_ERROR("Error message: %s", error_string);
```

### Timing
```cpp
// Plattform-unabhängige Zeitfunktionen
uint32_t current_time = AGGL_MILLIS();
AGGL_DELAY(100); // 100ms delay
```

### Mathematische Funktionen
```cpp
// Plattform-unabhängige Math-Funktionen
int min_val = AGGL_MIN(a, b);
int max_val = AGGL_MAX(a, b);
int abs_val = AGGL_ABS(value);
```

## ESP-IDF Integration

### Projekt Setup

1. **Component hinzufügen:**
   ```
   your_project/
   ├── components/
   │   └── aggl/
   │       ├── aggl_platform.h
   │       ├── AGGL.h
   │       ├── AGGL.cpp
   │       └── ... (andere AGGL Dateien)
   ├── main/
   │   └── main.c (oder .cpp)
   └── CMakeLists.txt
   ```

2. **CMakeLists.txt für Component:**
   ```cmake
   set(COMPONENT_SRCS
       "AGGL.cpp"
       "graphicsHandle.cpp"
       "textHandle.cpp"
       "imageTwoColorHandle.cpp"
       "image8BitHandle.cpp"
       "lineHandle.cpp"
       "rectangleHandle.cpp"
       "displayInterface.cpp"
       "optimizedRendering.cpp"
   )

   set(COMPONENT_ADD_INCLUDEDIRS ".")
   register_component()
   ```

3. **Main Component CMakeLists.txt:**
   ```cmake
   set(COMPONENT_REQUIRES aggl)
   ```

### ESP-IDF Display Implementierung

```cpp
#include "aggl_platform.h"
#include "AGGL.h"

class MyESPIDFDisplay : public AGGL::displayInterface
{
public:
    MyESPIDFDisplay(int16_t x, int16_t y, uint16_t w, uint16_t h, AGGL::COLOR_MODE::colormode mode)
        : displayInterface(x, y, w, h, mode) {}

    AGGL::STATUS::code init() override
    {
        // ESP-IDF spezifische Initialisierung
        AGGL_LOG("Initializing display");
        // SPI Bus setup, GPIO config, etc.
        return AGGL::STATUS::OK;
    }

    bool hasHardwareAcceleration() override
    {
        return true; // Wenn Display Hardware-Beschleunigung hat
    }

    AGGL::STATUS::code drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2,
                               int32_t color, uint8_t thickness) override
    {
        // Hardware-accelerated line drawing
        // Verwende ESP-IDF SPI Funktionen
        return AGGL::STATUS::OK;
    }
};
```

### Beispiel ESP-IDF main.c

```c
#include "aggl_platform.h"
#include "AGGL.h"

// Display Implementierung
class MyDisplay : public AGGL::displayInterface
{
    // ... Implementierung wie oben
};

extern "C" void app_main()
{
    // Display initialisieren
    MyDisplay* display = new MyDisplay(0, 0, 240, 320, AGGL::COLOR_MODE::RGB_16BIT);
    AGGL::addDisplay(display);
    AGGL::start();

    // Graphics Elemente erstellen
    AGGL::lineHandle* line = new AGGL::lineHandle(10, 10, 230, 310, AGGL::COLORS::RED, 3);
    AGGL::rectangleHandle* rect = new AGGL::rectangleHandle(50, 50, 140, 100,
                                                           AGGL::COLORS::BLUE,
                                                           AGGL::COLORS::WHITE, 2);

    line->show();
    rect->show();

    // Hauptloop
    while(true)
    {
        AGGL::updateWithHardwareAcceleration();
        AGGL_DELAY(16); // ~60 FPS
    }
}
```

## Migration von Arduino zu ESP-IDF

### Automatische Migration
Die meisten Änderungen sind automatisch durch die Plattform-Abstraktion abgedeckt. Code der die AGGL API verwendet bleibt unverändert.

### Manuelle Anpassungen
Falls Sie direkt Arduino-Funktionen verwendet haben:

**Vor (Arduino):**
```cpp
Serial.println("Debug message");
delay(100);
int min_val = min(a, b);
```

**Nach (ESP-IDF):**
```cpp
AGGL_LOG("Debug message");
AGGL_DELAY(100);
int min_val = AGGL_MIN(a, b);
```

## Performance Optimierungen

### ESP-IDF spezifische Vorteile
- **Native FreeRTOS Integration**: Optimierte Task-Scheduling
- **DMA Support**: Hardware-accelerated SPI Transfers
- **ESP32-S3 LCD Controller**: Dedicated Display Hardware
- **PSRAM Support**: Größere Framebuffer möglich

### Hardware-Beschleunigung
Nutzen Sie die Hardware-Beschleunigungs-Features für beste Performance:

```cpp
// Hardware-accelerated Rendering verwenden
AGGL::updateWithHardwareAcceleration();

// Anstatt des Standard-Updates
AGGL::update();
```

## Troubleshooting

### Häufige ESP-IDF Probleme

1. **SPI Bus nicht verfügbar:**
   ```
   E (123) spi: SPI bus already initialized
   ```
   Lösung: SPI Bus in anderer Reihenfolge initialisieren oder freigeben.

2. **Memory Allocation Fehler:**
   ```
   E (456) aggl: OOM
   ```
   Lösung: PSRAM aktivieren oder kleinere Buffer-Größen verwenden.

3. **GPIO Konflikte:**
   ```
   E (789) gpio: GPIO isr service already installed
   ```
   Lösung: GPIO Pins überprüfen und Konflikte vermeiden.

### Debug Logging

Aktivieren Sie detailliertes Logging in ESP-IDF:

```cpp
// In sdkconfig oder menuconfig
CONFIG_LOG_DEFAULT_LEVEL_INFO=y
CONFIG_LOG_MAXIMUM_LEVEL_DEBUG=y
```

## API Referenz

### Plattform-Abstraktion Makros

| Makro | Arduino | ESP-IDF | Beschreibung |
|-------|---------|---------|-------------|
| `AGGL_LOG(...)` | `Serial.printf(...)` | `ESP_LOGI(...)` | Info Logging |
| `AGGL_LOG_ERROR(...)` | `Serial.printf(...)` | `ESP_LOGE(...)` | Error Logging |
| `AGGL_DELAY(ms)` | `delay(ms)` | `vTaskDelay(...)` | Delay |
| `AGGL_MILLIS()` | `millis()` | `esp_timer_get_time()/1000` | Milliseconds |
| `AGGL_MIN(a,b)` | `min(a,b)` | `std::min(a,b)` | Minimum |
| `AGGL_MAX(a,b)` | `max(a,b)` | `std::max(a,b)` | Maximum |
| `AGGL_ABS(a)` | `abs(a)` | `std::abs(a)` | Absolute Value |

### Memory Management

```cpp
// Plattform-unabhängige Memory-Funktionen
void* buffer = AGGL_MALLOC(size);
AGGL_FREE(buffer);
AGGL_MEMCPY(dest, src, size);
```

## Beispiel-Projekte

- `espidf_example.cpp`: Vollständiges ESP-IDF Beispiel
- `exampleAcceleratedDisplay.cpp`: Hardware-Beschleunigung Beispiel
- ESP-IDF Component Template in `components/aggl/`

## Lizenz und Beiträge

Die AGGL-Bibliothek ist Open-Source. Beiträge für weitere Plattform-Unterstützung sind willkommen!