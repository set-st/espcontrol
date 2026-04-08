---
title: Specifications
description:
  Hardware specifications for the Guition ESP32-P4 touchscreen panels used by Espcontrol.
---

# Specifications

Espcontrol runs on **Guition ESP32** touchscreen panels designed for smart home control.

## Supported panels

### 7-inch — JC1060P470

| Spec | Value |
| --- | --- |
| Screen size | 7 inches |
| Resolution | 1024 x 600 pixels (landscape) |
| Display type | LCD (MIPI DSI) |
| Touch | Capacitive touchscreen |
| Backlight | Adjustable brightness (PWM) |
| Processor | ESP32-P4 |
| Layout | Fixed 4x5 grid (20 buttons) |

### 4.3-inch — JC4880P443

| Spec | Value |
| --- | --- |
| Screen size | 4.3 inches |
| Resolution | 480 x 800 pixels (portrait) |
| Display type | LCD (MIPI DSI) |
| Touch | Capacitive touchscreen |
| Backlight | Adjustable brightness (PWM) |
| Processor | ESP32-P4 |
| Layout | Fixed 5x3 grid (15 buttons) |

### 4-inch — 4848S040

| Spec | Value |
| --- | --- |
| Screen size | 4 inches |
| Resolution | 480 x 480 pixels (square) |
| Display type | LCD (SPI + RGB) |
| Touch | Capacitive touchscreen |
| Backlight | Adjustable brightness (PWM) |
| Processor | ESP32-S3 |
| Layout | Fixed 3x3 grid (9 buttons) |

## Processor and memory

The ESP32-P4 panels share the same SoC and memory configuration. The 4-inch panel uses an ESP32-S3.

| Spec | Value |
| --- | --- |
| Main processor | ESP32-P4 |
| WiFi | ESP32-C6 (built-in, 2.4 GHz) |
| Flash storage | 16 MB |
| RAM | PSRAM (high-speed) |

## Connectivity

| Feature | Details |
| --- | --- |
| WiFi | 2.4 GHz, connects to your home network |
| Home Assistant | Native API (automatic discovery) |
| USB | USB-C port (for initial flashing and power) |
| Over-the-air updates | Yes, via WiFi |
| Web interface | Built-in, accessible from any browser on your network |

## Power

Both panels are powered through the USB-C port. A standard USB-C phone charger (5V) is sufficient.

## Where to buy

- **7-inch panel (JC1060P470):** [AliExpress](https://s.click.aliexpress.com/e/_c335W0r5) (~£40)
- **4.3-inch panel (JC4880P443):** [AliExpress](https://s.click.aliexpress.com/e/_c3nNpYXn) (~£24)
- **Desk stand for 7-inch** (3D printable): [MakerWorld](https://makerworld.com/en/models/2387421-guition-esp32p4-jc1060p470-7inch-screen-desk-mount#profileId-2614995)
