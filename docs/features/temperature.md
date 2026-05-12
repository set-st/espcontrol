---
title: EspControl Temperature Display
description:
  How to show indoor and outdoor temperatures on your EspControl panel's top bar.
---

# Temperature Display

The top bar of your panel can show indoor and outdoor temperatures from any temperature sensor in Home Assistant. You'll find the sensor toggles in the **Clock Bar** section of the **Settings** tab on the [Setup](/features/setup) page.

- **Indoor Temperature** — turn it on, then choose which sensor to use (for example, a room thermometer).
- **Outdoor Temperature** — works the same way. Use a weather sensor or outdoor thermometer.
- **Show Degree Symbol** — turn this off if you want the clock bar to show only the temperature numbers, with no `°C` or `°F` suffix.
- **Temperature Unit** — choose **Auto**, **°C**, or **°F** in the **Temperature** section for the temperature labels. Auto uses the timezone setting and selects °F for US and US-territory timezones, otherwise °C.

When both are turned on, the top bar shows two temperatures side by side. When only one is turned on, it shows a single value.

If you turn off **Show Clock Bar** in the **Clock Bar** settings, the temperature display is hidden along with the top bar.
