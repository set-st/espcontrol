---
title: 7-inch Waveshare ESP32-S3
description:
  EspControl on the Waveshare ESP32-S3-Touch-LCD-7 — a 7-inch 1024x600 landscape touchscreen with 15 cards, powered by ESP32-S3.
---

# 7-inch Waveshare S3

![EspControl on a 7-inch touchscreen: home screen with temperature, clock, and control tiles](/images/home_screen_hero.jpg)

The **Waveshare ESP32-S3-Touch-LCD-7** is a 7-inch landscape touchscreen powered by an **ESP32-S3** processor. It has a 1024×600 display and room for **15 cards** on the home screen.

## Specifications

| | |
|---|---|
| **Screen size** | 7 inches |
| **Resolution** | 1024 × 600 |
| **Orientation** | Landscape |
| **Display interface** | RGB |
| **Processor** | ESP32-S3 (240 MHz) |
| **WiFi** | Built-in (2.4 GHz) |
| **Flash** | 16 MB |
| **PSRAM** | Octal mode, 80 MHz |
| **Touch** | GT911 capacitive |
| **Power** | USB-C |

## Card Grid

The home screen uses a **3-row × 5-column** grid, giving you **15 card slots**. Any home-screen card can be turned into a [Subpage](/features/subpages) folder containing up to 14 more cards, so you can organise far more than 15 controls without cluttering the home screen.

Flexible card sizes are supported: Single, Tall, Wide, and Large.

## Install

Connect the display to your computer with a USB-C data cable, then click the button to flash.

<EspInstallButton slug="waveshare-esp32-s3-1024x600" />

For a full walkthrough including WiFi setup and Home Assistant pairing, see the [Install guide](/getting-started/install).

## ESPHome Manual Setup

If you use ESPHome and prefer to compile firmware yourself:

```yaml
substitutions:
  name: "office-screen"
  friendly_name: "Office Screen"

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

packages:
  setup:
    url: https://github.com/set-st/espcontrol/
    file: devices/waveshare-esp32-s3-1024x600/packages.yaml
    refresh: 1sec
```

For the full manual setup notes, see [Manual ESPHome Setup](/getting-started/manual-esphome-setup).

## Where to Buy

- **Waveshare:** [ESP32-S3-Touch-LCD-7](https://www.waveshare.com/esp32-s3-touch-lcd-7.htm)
