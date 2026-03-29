---
title: External ESPHome component
description:
  espcontrol custom component from this repo — sun_calc.h for timezone coordinates and sunrise or sunset on the device.
---

# External ESPHome component

[`device/device.yaml`](https://github.com/jtenniswood/espcontrol/blob/main/devices/guition-esp32-p4-jc1060p470/device/device.yaml) registers an **external_components** source pointing at this repository:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/jtenniswood/espcontrol
      ref: main
      path: components
    components: [espcontrol]
    refresh: 0s

espcontrol:
```

## Python layer

[`components/espcontrol/__init__.py`](https://github.com/jtenniswood/espcontrol/blob/main/components/espcontrol/__init__.py) currently defines an empty **`CONFIG_SCHEMA`** and **`to_code`** — the package is present so ESPHome accepts the `espcontrol:` key and can link C++ headers.

## C++ / headers

[`components/espcontrol/sun_calc.h`](https://github.com/jtenniswood/espcontrol/blob/main/components/espcontrol/sun_calc.h) provides:

- **`TzCoord`** table — representative **lat/lon** for each **IANA timezone** option matching **`Clock: Timezone`** in [`addon/time.yaml`](https://github.com/jtenniswood/espcontrol/blob/main/common/addon/time.yaml)
- **`lookup_tz_coords`** — resolve coordinates for a timezone id string
- **`calc_sunrise_sunset`** — astronomical sunrise/sunset for a calendar date and position (used by [`addon/backlight_schedule.yaml`](https://github.com/jtenniswood/espcontrol/blob/main/common/addon/backlight_schedule.yaml) lambdas)

This keeps **sunrise/sunset** and **day/night brightness** on-device **without** calling external APIs.

## Related

- [Backlight schedule](/backlight-schedule) — how computed times drive brightness
