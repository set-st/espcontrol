---
title: EspControl Backlight Settings
description:
  How the EspControl panel automatically adjusts screen brightness during the day and night based on sunrise and sunset.
---

# Backlight

The panel automatically adjusts screen brightness based on time of day — brighter during daylight, dimmer at night.

## How It Works

Sunrise and sunset times are calculated on-device from your selected timezone using a NOAA solar algorithm. During the day, the panel uses your **Daytime Brightness**; at night, it switches to **Nighttime Brightness**. The transition is checked every 60 seconds, and sunrise/sunset are recalculated at midnight. No internet connection or Home Assistant is required.

## Settings

Configured in the **Brightness** section of the **Settings** tab in [Setup](/features/setup).

- **Daytime Brightness** — screen brightness during the day (10%–100%, default 100%).
- **Nighttime Brightness** — screen brightness at night (10%–100%, default 75%).

Sunrise and sunset times are derived from the timezone set in [Time Settings](/features/clock).

## Screensaver

When the screensaver uses **Screen Dimmed**, it keeps the normal screen visible at the saved dim brightness. When the screensaver clock is active, it can use separate daytime and nighttime clock brightness values based on the same sunrise and sunset times. If the screensaver is set to Display Off, the backlight turns off completely. On wake (touch or presence sensor), brightness returns to the correct level for the current time.

## Screen Schedule

The [screen schedule](/features/screen-schedule) can turn the physical backlight off, keep the panel dimmed, or show a clock at set hours. **Screen Off** uses the schedule's separate **When Woken** brightness during a temporary wake. **Screen Dimmed** uses its own overnight brightness setting. **Clock** uses its own clock brightness setting.

## Before Clock Sync

If the panel hasn't synced its clock yet, it defaults to daytime brightness. Once synced, sunrise and sunset are calculated immediately.
