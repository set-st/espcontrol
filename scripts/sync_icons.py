#!/usr/bin/env python3
"""Synchronize icon data from icons.json into downstream files.

Usage:
    python scripts/sync_icons.py           # write mode (default)
    python scripts/sync_icons.py --check   # exit 1 if any file needs updating
"""
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

SOURCE = ROOT / "common" / "assets" / "icons.json"

DEVICES = [
    {"slug": "guition-esp32-p4-jc1060p470"},
    {"slug": "guition-esp32-p4-jc4880p443"},
    {"slug": "guition-esp32-s3-4848s040"},
]

SHARED_TARGETS = {
    "icons_yaml": ROOT / "common" / "assets" / "icons.yaml",
}


def load_icons():
    with open(SOURCE) as f:
        return json.load(f)


def replace_between_markers(text, start_tag, end_tag, new_content):
    """Replace content between marker lines, preserving the markers themselves."""
    pattern = re.compile(
        r"(^[^\n]*" + re.escape(start_tag) + r"[^\n]*\n)"
        r"(.*?)"
        r"(^[^\n]*" + re.escape(end_tag) + r"[^\n]*$)",
        re.MULTILINE | re.DOTALL,
    )
    m = pattern.search(text)
    if not m:
        raise ValueError(f"Markers not found: {start_tag} / {end_tag}")
    return text[: m.start(2)] + new_content + text[m.start(3) :]


# ---------------------------------------------------------------------------
# Generators for each target file
# ---------------------------------------------------------------------------

def gen_icons_yaml_glyphs(data):
    """Font glyph codepoint list for LVGL font subsetting."""
    fb = data["fallback"]
    lines = [f'      - "\\U{fb["codepoint"]:>08s}"  # mdi-{fb["mdi"]} (Auto fallback)\n']
    for icon in data["icons"]:
        cp = icon["codepoint"]
        lines.append(f'      - "\\U{cp:>08s}"  # mdi-{icon["mdi"]}\n')
    return "".join(lines)


def gen_sensors_icon_entries(data):
    """C++ icon_entries[] struct initializers."""
    max_name_len = max(len(i["name"]) for i in data["icons"])
    lines = []
    for icon in data["icons"]:
        padded = f'"{icon["name"]}",'
        padded = padded.ljust(max_name_len + 3)
        lines.append(f'              {{{padded} "\\U{icon["codepoint"]:>08s}"}},\n')
    return "".join(lines)


def gen_sensors_domain_icons(data):
    """C++ if/else if chain for domain default icons."""
    icon_by_name = {i["name"]: i for i in data["icons"]}
    entries = list(data["domain_defaults"].items())
    # Align icon_cp at column 54 to match existing style
    target_col = 54
    lines = []
    for i, (domain, icon_name) in enumerate(entries):
        icon = icon_by_name[icon_name]
        cp = icon["codepoint"]
        keyword = "if      " if i == 0 else "else if "
        prefix = f'                  {keyword}(domain == "{domain}")'
        pad = max(target_col - len(prefix), 1)
        lines.append(
            f'{prefix}{" " * pad}'
            f'icon_cp = "\\U{cp:>08s}";  // {icon_name}\n'
        )
    return "".join(lines)


def gen_www_js_icon_map(data):
    """JS ICON_MAP object entries."""
    lines = []
    for icon in data["icons"]:
        name = icon["name"]
        mdi = icon["mdi"]
        # Use unquoted key if it's a valid JS identifier
        if re.match(r"^[A-Za-z_$][A-Za-z0-9_$]*$", name):
            lines.append(f'    {name}: "{mdi}",\n')
        else:
            lines.append(f'    "{name}": "{mdi}",\n')
    return "".join(lines)


def gen_www_js_domain_icons(data):
    """JS DOMAIN_ICONS object entries."""
    icon_by_name = {i["name"]: i for i in data["icons"]}
    lines = []
    for domain, icon_name in data["domain_defaults"].items():
        mdi = icon_by_name[icon_name]["mdi"]
        lines.append(f'    {domain}: "{mdi}",\n')
    return "".join(lines)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def sync(check_only=False):
    data = load_icons()
    dirty = []

    patches = [
        (
            SHARED_TARGETS["icons_yaml"],
            "GENERATED:ICONS START",
            "GENERATED:ICONS END",
            gen_icons_yaml_glyphs,
        ),
    ]

    for device in DEVICES:
        slug = device["slug"]
        sensors_path = ROOT / "devices" / slug / "device" / "sensors.yaml"
        www_js_path = ROOT / "docs" / "public" / "webserver" / slug / "www.js"

        patches.extend([
            (sensors_path, "GENERATED:ICONS START", "GENERATED:ICONS END", gen_sensors_icon_entries),
            (sensors_path, "GENERATED:DOMAIN_ICONS START", "GENERATED:DOMAIN_ICONS END", gen_sensors_domain_icons),
            (www_js_path, "GENERATED:ICONS START", "GENERATED:ICONS END", gen_www_js_icon_map),
            (www_js_path, "GENERATED:DOMAIN_ICONS START", "GENERATED:DOMAIN_ICONS END", gen_www_js_domain_icons),
        ])

    file_contents = {}

    for path, start_tag, end_tag, generator in patches:
        if path not in file_contents:
            file_contents[path] = path.read_text()

        old = file_contents[path]
        new_content = generator(data)
        updated = replace_between_markers(old, start_tag, end_tag, new_content)

        if updated != old:
            file_contents[path] = updated
            rel = path.relative_to(ROOT)
            dirty.append((rel, start_tag))

    if check_only:
        if dirty:
            print("Icon data is out of sync. Run 'python scripts/sync_icons.py' to fix:")
            for rel, tag in dirty:
                print(f"  {rel} ({tag})")
            return 1
        print("Icon data is in sync.")
        return 0

    for path, content in file_contents.items():
        original = path.read_text()
        if content != original:
            path.write_text(content)
            print(f"  updated {path.relative_to(ROOT)}")

    if not dirty:
        print("All files already in sync.")
    else:
        print(f"Synced {len(dirty)} section(s).")
    return 0


if __name__ == "__main__":
    check = "--check" in sys.argv
    sys.exit(sync(check_only=check))
