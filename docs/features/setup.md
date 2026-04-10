---
title: Setup
description:
  How to use the built-in web page to configure buttons, icons, display settings, screensaver, and brightness on your Espcontrol panel.
---

# Setup

Your Espcontrol panel has a built-in web page where you can set everything up. Open it by typing the panel's address into any browser on your phone or computer.

::: tip Finding the address
The address is shown on the display screen when no buttons are configured yet. You can also find it in your router's connected devices list, or in Home Assistant under **Settings > Devices & Services > ESPHome**.
:::

## Buttons

Your panel has a grid of button spaces — **20** on the 7-inch, **15** on the 4.3-inch, or **9** on the 4-inch.

### Adding a button

Tap any empty space in the grid (shown as a dashed outline with a **+** icon). A settings panel appears below the preview where you configure the button:

1. **Choose a type** — **Toggle** (the default) to control a device, or **Subpage** to create a folder of extra buttons.
2. **Pick the device** you want to control by entering its Home Assistant entity name (for example, `light.living_room` or `switch.garden_lights`). You can find these under **Settings > Devices & Services** in Home Assistant. Subpage buttons don't need a device name.
3. **Choose an icon** — type to search, or select **Auto** to let the panel pick one based on the device type.
4. **Set a label** (optional). If left blank the button uses the device's friendly name from Home Assistant.

### Button types

**Toggle** — connected to a Home Assistant device. Tap it on the panel to switch the device on or off. The button changes colour to show the current state.

**Subpage** — works like a folder. Tapping it opens a new page with its own set of buttons, great for grouping related controls without filling up the home screen. The subpage has one fewer slot than the home screen because the first slot is a **Back** button. Subpage buttons show a small **arrow badge** on the home screen.

To configure a subpage, click **Configure Subpage** in the button settings, or right-click the button and choose **Edit Subpage**. Add and arrange buttons there the same way you would on the home screen, then click the back arrow to return.

### When Entity On

Each toggle button has an optional **When Entity On** setting that changes what the button shows while the device is on:

- **Replace Icon** — show a different icon when the device is on (for example, an outline lightbulb when off and a filled one when on).
- **Sensor Data** — show a live reading when the device is on (for example, temperature, power usage, or a percentage). Pick the **Sensor** entity and a **Unit** (`%`, `°C`, `W`, etc.).

When the device is off, the button reverts to its normal icon.

### Selecting buttons

- **Tap** a button to select it and show its settings.
- **Shift+click** to select a range.
- **Ctrl+click** (Cmd+click on Mac) to toggle individual buttons.

When multiple buttons are selected, right-click to cut or delete them all at once.

### Moving buttons

Drag and drop any button to reposition it. If you drop it onto an occupied space, the existing button shifts to the next available slot.

### Double-height buttons

Right-click a button and choose **Double Height** to make it span two rows. To revert, right-click and choose **Single Height**. If a button already occupies the space below, it gets moved automatically.

### Right-click menu

Right-click a button to see:

| Option | What it does |
| --- | --- |
| **Edit Subpage** | Opens the subpage editor (Subpage buttons only) |
| **Double Height** / **Single Height** | Toggles between one-row and two-row height |
| **Duplicate** | Copies the button to the next available space |
| **Cut** | Removes the button so you can paste it elsewhere |
| **Delete** | Removes the button permanently |

Right-click an **empty space** to **Paste** a previously cut button. Cut and paste also works between the home screen and subpages.

## Apply Configuration

After making changes, tap **Apply Configuration** at the bottom of the page. The panel restarts and loads your new settings — you'll see a message while it reconnects.
