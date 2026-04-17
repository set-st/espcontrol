![Espcontrol on a 7-inch touchscreen: home screen with temperature, clock, and control tiles](docs/public/images/home_screen_hero.jpg)

# Espcontrol

**A no-code, super-easy-to-configure smart home controller.** Configure affordable touchscreens to control devices across your entire smart home — no ESPHome setup, no YAML, no code to write. Just flash, connect, and start adding buttons.

Espcontrol is free, open-source firmware that turns **Guition ESP32** touchscreens into beautiful control panels for [Home Assistant](https://www.home-assistant.io/). It comes with **full documentation** and an **easy-to-use web installer** — you can go from unboxing to a working wall panel in minutes.

Supported panels include the **7-inch JC1060P470** (1024×600, landscape, 15 buttons), the **4.3-inch JC4880P443** (480×800, portrait, 15 buttons), and the **4-inch 4848S040** (480×480, square, 9 buttons). Each panel uses a fixed grid layout sized to its screen, plus a status bar with a clock and temperatures, a screensaver, automatic brightness, and **over-the-air updates**. After the first install, everything is configured through the device's **built-in web page**.

**Documentation and install guide:** [set-st.github.io/espcontrol](https://set-st.github.io/espcontrol/)

## Features

- **Grid layout** — a fixed grid sized to each screen so you can place buttons exactly where you want them
- **Subpages** — group related controls into folders to keep the home screen tidy
- **Double-height buttons** — span any button across two rows for easier tapping and greater prominence
- **Edit controls** — drag-and-drop ordering, bulk select, and copy-paste between pages from the built-in web UI
- **Screensaver** — dims and sleeps after a set time, or wakes automatically from a presence sensor

![Web-based button configuration on the 4-inch 4848S040](docs/public/images/4848s040-buttons.png)

## Supported screens

| 7″ JC1060P470 | 4.3″ JC4880P443 | 4″ 4848S040 |
|:-:|:-:|:-:|
| ![7-inch JC1060P470](docs/public/images/jc1060p470-hero.png) | ![4.3-inch JC4880P443](docs/public/images/jc4880p443-hero.png) | ![4-inch 4848S040](docs/public/images/4848s040-hero.png) |
| 1024×600 landscape · 15 buttons | 480×800 portrait · 15 buttons | 480×480 square · 9 buttons |
| ESP32-P4 | ESP32-P4 | ESP32-S3 |
| [AliExpress ~£40](https://s.click.aliexpress.com/e/_c335W0r5) | [AliExpress ~£24](https://s.click.aliexpress.com/e/_c32jr3eN) | [AliExpress ~£16](https://s.click.aliexpress.com/e/_c3sIhvBv) |
| [Desk stand (3D print)](https://makerworld.com/en/models/2387421-guition-esp32p4-jc1060p470-7inch-screen-desk-mount#profileId-2614995) | | [Case stand (3D print)](https://makerworld.com/en/models/2581572-guition-esp32s3-4848s040-case-stand#profileId-2847301) |

See the [docs](https://set-st.github.io/espcontrol/) for full specs and install instructions for each screen.

## Getting started

1. **Buy a panel** (see above)
2. **Flash the firmware** from your browser — follow the [install guide](https://set-st.github.io/espcontrol/getting-started/install)
3. **Connect to WiFi** using the on-screen setup
4. **Add to Home Assistant** — it will be discovered automatically
5. **Allow Home Assistant actions** — [enable the device](https://set-st.github.io/espcontrol/getting-started/home-assistant-actions) to control your entities
6. **Configure your buttons** by opening the panel's built-in web page

## Links

- [Documentation](https://set-st.github.io/espcontrol/)
- [Install guide](https://set-st.github.io/espcontrol/getting-started/install)
- [FAQ](https://set-st.github.io/espcontrol/reference/faq)
- [Report a bug or request a feature](https://github.com/set-st/espcontrol/issues)
