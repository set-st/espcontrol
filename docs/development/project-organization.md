---
title: Project Organization
description: Maintainer notes for web UI modules, generated files, device metadata, and firmware UI headers.
---

# Project Organization

This page explains where to make common changes and which files are generated. The goal is to keep the project easy to review: edit the human-owned source, regenerate the derived files, then run the checks.

## Source and Generated Files

Do not hand-edit generated files unless you are debugging a generator.

| Area | Human-edited source | Generated output |
|---|---|---|
| Device metadata | `devices/manifest.json` | Web device config inside `docs/public/webserver/*/www.js` and repeated YAML in `devices/*/device/sensors.yaml` / `packages.yaml` |
| Web UI | `src/webserver/www.js`, `src/webserver/modules/*.js`, `src/webserver/types/*.js` | `docs/public/webserver/<device>/www.js` |
| Icons | `common/assets/icons.json` | `components/espcontrol/icons.h`, `common/assets/icon_glyphs.yaml`, icon blocks in web output |

Generated web bundles are marked in `.gitattributes` so reviews can focus on the source files first.

## Web UI Layout

`src/webserver/www.js` is the browser entry point. It keeps the device config markers, generated icon markers, and test hooks in one place.

The larger UI code lives in `src/webserver/modules`:

| Module | Responsibility |
|---|---|
| `styles.js` | Injected CSS and visual tokens |
| `state.js` | Shared editor state, default values, import/export helpers |
| `api.js` | ESPHome web API calls and entity loading |
| `grid.js` | Screen preview grid, drag/drop, sizing, and subpage grid behavior |
| `config_codec.js` | Saved card config parsing and serialization |
| `controls.js` | Reusable form controls for card editors |
| `app.js` | App startup, tab wiring, settings panels, and save flow |

Card type editors live in `src/webserver/types`. New editors should use the helpers in `controls.js` where possible, especially `entityField`, `selectField`, `segmentControl`, `toggleSection`, and `precisionField`. That keeps labels, layout, and saved config behavior consistent.

`scripts/web_source.js` is the shared loader used by tests. `scripts/build.py` uses the same module order when creating each device's single `www.js` file.

## Device Manifest

`devices/manifest.json` is the device source of truth for:

- slot count and grid shape
- web preview sizing
- rotation support
- internal relay count
- display color and width compensation
- firmware font IDs used by generated grid setup

When adding or changing a device, update the manifest first. Then run:

```sh
python3 scripts/generate_device_slots.py
python3 scripts/build.py
```

The generated YAML comments point back to the manifest. If a generated file looks wrong, fix the manifest or generator rather than editing the output.

## Firmware UI Headers

Device YAML should include `components/espcontrol/button_grid.h`. Keep that file as the public compatibility include.

The implementation is split into smaller internal headers:

| Header | Responsibility |
|---|---|
| `button_grid_config.h` | Config parsing, field normalization, text helpers, entity helpers |
| `button_grid_layout.h` | Color parsing, width compensation, order parsing, label wrapping |
| `button_grid_cards.h` | Card visual setup for sensors, weather, locks, garage doors, internal relays, and subpage parent cards |
| `button_grid_subscriptions.h` | Home Assistant state subscriptions |
| `button_grid_actions.h` | Home Assistant action dispatch and button click handling |
| `button_grid_sliders.h` | Slider cards, cover sliders, light temperature sliders, and media volume modal |
| `button_grid_climate.h` | Climate card and modal behavior |
| `button_grid_confirm.h` | Switch turn-off confirmation modal behavior |
| `button_grid_media.h` | Media player status, metadata, progress, and controls |
| `button_grid_subpages.h` | Subpage config parsing and dynamic subpage slot setup |
| `button_grid_grid.h` | Main grid setup phases called from generated YAML |

Prefer changing the narrow internal header that owns the behavior. Only change `button_grid.h` when the public include list changes.

## Change Checklist

Before committing, run the local checks:

```sh
npm run check:all
```

For release-quality firmware changes, also compile the supported factory builds:

```sh
docker run --rm -v "${PWD}:/config" ghcr.io/esphome/esphome:2026.4.5 compile /config/builds/guition-esp32-p4-jc1060p470.factory.yaml
docker run --rm -v "${PWD}:/config" ghcr.io/esphome/esphome:2026.4.5 compile /config/builds/guition-esp32-p4-jc4880p443.factory.yaml
docker run --rm -v "${PWD}:/config" ghcr.io/esphome/esphome:2026.4.5 compile /config/builds/guition-esp32-p4-jc8012p4a1.factory.yaml
docker run --rm -v "${PWD}:/config" ghcr.io/esphome/esphome:2026.4.5 compile /config/builds/esp32-p4-86.factory.yaml
docker run --rm -v "${PWD}:/config" ghcr.io/esphome/esphome:2026.4.5 compile /config/builds/guition-esp32-s3-4848s040.factory.yaml
```

If a generated check fails, run the generator it names, review the source and generated diff together, then rerun `npm run check:all`.
