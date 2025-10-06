# AGGL - Cross-Platform Graphics Library

Another Goddam Graphics Library

Eine plattformübergreifende Grafikbibliothek für Mikrocontroller mit Unterstützung für Arduino und ESP-IDF.

## 🚀 Features

- **Plattformübergreifend**: Arduino & ESP-IDF kompatibel
- **Hardware-Beschleunigung**: Native Display-Funktionen nutzen
- **Transparenz**: Für Bilder via Transparenz-Schlüssel
- **Partielle Updates**: Für Displays mit partiellen Update-Fähigkeiten
- **Objektbasiertes System**: Einfache Verwaltung von Grafikelementen
- **Minimaler RAM-Verbrauch**: Anpassbare RAM-Limits
- **U8Glib Font-Unterstützung**: Kompatibel mit existierenden Fonts
- **N-Farb Display-Unterstützung**: Flexibel für verschiedene Display-Typen
- **Dithering**: Für Monochrom-Displays
- **Font-Skalierung**: Vergrößerung kleiner Fonts bei Flash-Limitierungen
- **Benutzerdefinierte Grafikelemente**: Eigene Elemente ableiten

## 📦 Unterstützte Plattformen

### Arduino
- AVR, SAM, ESP32 (Arduino Framework)
- Automatische Erkennung

### ESP-IDF
- Native ESP32-Serie Unterstützung
- FreeRTOS Integration
- Hardware-Beschleunigung für ESP32-S3 LCD Controller

## 📋 API Übersicht

### TEXT

- Show/Hide
- Alter Text
- Alter Position
- Alter Font
- Width Limit (NONE, WRAP, HIDE_OVERFLOW)

```c++
AGGL::textHandle* myText = new AGGL::textHandle(10, 20, "Hello World", u8g2_font_6x13_mf);
if(myText)
{
  myText->show();
  AGGL::update();

  myText->hide();
  AGGL::update();

  myText->changePosition(20, 30);
  AGGL::update();

  myText->changeText("What's going on?");
  AGGL::update();
}
```

### LINE

- Show/Hide
- Alter Positions
- Alter Color
- Alter Thickness
- Hardware-Beschleunigung

```c++
AGGL::lineHandle* myLine = new AGGL::lineHandle(10, 20, 30, 40, AGGL::COLORS::RED, 2);
if(myLine)
{
  myLine->show();
  AGGL::update();

  myLine->hide();
  AGGL::update();

  myLine->changeLine(20, 30, 40, 50);
  AGGL::update();

  myLine->setColor(AGGL::COLORS::BLUE);
  AGGL::update();
}
```

### RECTANGLE

- Show/Hide
- Alter Position/Größe
- Füllfarbe & Rahmenfarbe
- Rahmenstärke
- Hardware-Beschleunigung

```c++
AGGL::rectangleHandle* myRect = new AGGL::rectangleHandle(10, 20, 100, 50,
                                                         AGGL::COLORS::BLUE,
                                                         AGGL::COLORS::WHITE, 2);
if(myRect)
{
  myRect->show();
  AGGL::update();

  myRect->changeRectangle(20, 30, 120, 60);
  AGGL::update();

  myRect->setFillColor(AGGL::COLORS::GREEN);
  AGGL::update();
}
```

## 🛠 Installation & Setup

### Arduino
1. Kopieren Sie alle `.cpp` und `.h` Dateien in Ihren Arduino Sketch-Ordner
2. Fügen Sie `#include "AGGL.h"` zu Ihrem Sketch hinzu

### ESP-IDF
1. Erstellen Sie einen `components/aggl/` Ordner in Ihrem ESP-IDF Projekt
2. Kopieren Sie alle AGGL-Dateien in diesen Ordner
3. Fügen Sie die Component zu Ihrer `CMakeLists.txt` hinzu:

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

## 📚 Dokumentation

- **[Hardware Acceleration Guide](HARDWARE_ACCELERATION.md)**: Optimierung für Hardware-Beschleunigung
- **[ESP-IDF Integration](ESP_IDF_INTEGRATION.md)**: Detaillierte ESP-IDF Anleitung

## 🔧 Beispiel

```c++
// Display initialisieren
MyDisplay* display = new MyDisplay(0, 0, 240, 320, AGGL::COLOR_MODE::RGB_16BIT);
AGGL::addDisplay(display);
AGGL::start();

// Elemente erstellen
auto* line = new AGGL::lineHandle(10, 10, 230, 310, AGGL::COLORS::RED, 3);
auto* rect = new AGGL::rectangleHandle(50, 50, 140, 100, AGGL::COLORS::BLUE);

line->show();
rect->show();

// Hardware-beschleunigtes Update
AGGL::updateWithHardwareAcceleration();
```

## 📄 Lizenz

Open-Source - Beiträge willkommen!
