// =============================================================================
// BUTTON GRID - LVGL button grid layout, parsing, and HA entity binding
// =============================================================================
// Shared C++ utilities included by each device's sensors.yaml lambda. Handles:
//   - Parsing semicolon-delimited button config strings into structured fields
//   - Grid layout with double-height (d), double-wide (w), and 2×2 big (b) cells
//   - LVGL visual setup for toggle buttons, sensor cards, and slider widgets
//   - Home Assistant state subscriptions and action dispatch
//   - Subpage creation (nested grid screens with back button)
// =============================================================================
#pragma once
#include <string>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <vector>
#include <functional>
#include "icons.h"
#include "backlight.h"

constexpr uint32_t DEFAULT_SLIDER_COLOR = 0xFF8C00;

// LVGL widget handles for one button slot on the main grid
struct BtnSlot {
  esphome::text::Text *config;       // persisted config string (entity;label;icon;...)
  lv_obj_t *btn;                     // button container
  lv_obj_t *icon_lbl;               // icon label (MDI glyph)
  lv_obj_t *text_lbl;               // entity name / custom label
  lv_obj_t *sensor_container;       // flex row shown when sensor overlay is active
  lv_obj_t *sensor_lbl;             // numeric sensor value
  lv_obj_t *unit_lbl;               // unit suffix (°C, %, etc.)
};

// Extract the Nth semicolon-delimited field from a config string
inline std::string cfg_field(const std::string &cfg, int idx) {
  size_t start = 0;
  for (int i = 0; i < idx; i++) {
    size_t pos = cfg.find(';', start);
    if (pos == std::string::npos) return "";
    start = pos + 1;
  }
  size_t end = cfg.find(';', start);
  return (end == std::string::npos) ? cfg.substr(start) : cfg.substr(start, end - start);
}

// Structured view of a button config string: entity;label;icon;icon_on;sensor;unit;type
struct ParsedCfg {
  std::string entity;   // 0  HA entity_id (e.g. light.kitchen)
  std::string label;    // 1  display name (blank = use HA friendly_name)
  std::string icon;     // 2  icon name for off/default state
  std::string icon_on;  // 3  icon name for on state (blank = no swap)
  std::string sensor;   // 4  sensor entity for toggle overlay; "h" for horizontal slider
  std::string unit;     // 5  unit suffix for sensor display
  std::string type;     // 6  button type: "" (toggle), sensor, slider, cover, push, subpage
};

inline ParsedCfg parse_cfg(const std::string &cfg) {
  ParsedCfg p;
  p.entity  = cfg_field(cfg, 0);
  p.label   = cfg_field(cfg, 1);
  p.icon    = cfg_field(cfg, 2);
  p.icon_on = cfg_field(cfg, 3);
  p.sensor  = cfg_field(cfg, 4);
  p.unit    = cfg_field(cfg, 5);
  p.type    = cfg_field(cfg, 6);
  return p;
}

// Parse a 6-char hex color string (no # prefix) into a uint32_t RGB value
inline uint32_t parse_hex_color(const std::string &hex, bool &valid) {
  valid = hex.length() == 6;
  if (!valid) return 0;
  return strtoul(hex.c_str(), nullptr, 16);
}

// Reduce green channel to 80% to compensate for display panel color shift
inline uint32_t correct_color(uint32_t rgb) {
  uint8_t r = (rgb >> 16) & 0xFF;
  uint8_t g = (uint8_t)(((rgb >> 8) & 0xFF) * 80 / 100);
  uint8_t b = rgb & 0xFF;
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

// ── Grid layout parsing ───────────────────────────────────────────────

// Result of parsing a button_order CSV string into grid cell positions
struct OrderResult {
  int positions[25] = {};    // slot number at each grid position (1-based, 0=empty)
  bool is_double[25] = {};   // slot uses double height (suffix "d" or "b")
  bool is_wide[25] = {};     // slot uses double width (suffix "w" or "b")
};

// Parse "1,2d,3w,4b,..." into positions + double/wide flags
inline void parse_order_string(const std::string &order_str, int num_slots, OrderResult &result) {
  memset(result.positions, 0, sizeof(result.positions));
  memset(result.is_double, 0, sizeof(result.is_double));
  memset(result.is_wide, 0, sizeof(result.is_wide));
  if (order_str.empty()) return;
  size_t gpos = 0, start = 0;
  while (start <= order_str.length() && gpos < (size_t)num_slots) {
    size_t comma = order_str.find(',', start);
    if (comma == std::string::npos) comma = order_str.length();
    if (comma > start) {
      std::string token = order_str.substr(start, comma - start);
      bool dbl = !token.empty() && token.back() == 'd';
      bool wide = !token.empty() && token.back() == 'w';
      bool big = !token.empty() && token.back() == 'b';
      if (big) { dbl = true; wide = true; }
      if (dbl || wide) token.pop_back();
      int v = atoi(token.c_str());
      if (v >= 1 && v <= num_slots) {
        result.positions[gpos] = v;
        result.is_double[v - 1] = dbl;
        result.is_wide[v - 1] = wide;
      }
    }
    gpos++;
    start = comma + 1;
  }
}

// Zero out grid cells that are covered by a neighbouring double/wide/big button
inline void clear_spanned_cells(const OrderResult &order, int num_slots, int cols, OrderResult &result) {
  for (int p = 0; p < num_slots; p++) {
    result.positions[p] = order.positions[p];
    result.is_double[p] = order.is_double[p];
    result.is_wide[p] = order.is_wide[p];
  }
  for (int p = 0; p < num_slots; p++) {
    if (result.positions[p] <= 0) continue;
    int idx = result.positions[p] - 1;
    if (result.is_double[idx] && p + cols < num_slots) {
      result.positions[p + cols] = 0;
    }
    if (result.is_wide[idx] && (p + 1) % cols != 0 && p + 1 < num_slots) {
      result.positions[p + 1] = 0;
    }
    if (result.is_double[idx] && result.is_wide[idx] && (p + 1) % cols != 0 && p + cols + 1 < num_slots) {
      result.positions[p + cols + 1] = 0;
    }
  }
}

// ── Button visuals ────────────────────────────────────────────────────

// Apply on/off background colors to a button's checked/pressed/default states
inline void apply_button_colors(lv_obj_t *btn, bool has_on, uint32_t on_val,
                                bool has_off, uint32_t off_val) {
  if (has_on) {
    lv_obj_set_style_bg_color(btn, lv_color_hex(on_val),
      static_cast<lv_style_selector_t>(LV_PART_MAIN) | static_cast<lv_style_selector_t>(LV_STATE_CHECKED));
    lv_obj_set_style_bg_color(btn, lv_color_hex(on_val),
      static_cast<lv_style_selector_t>(LV_PART_MAIN) | static_cast<lv_style_selector_t>(LV_STATE_PRESSED));
  }
  if (has_off) {
    lv_obj_set_style_bg_color(btn, lv_color_hex(off_val),
      static_cast<lv_style_selector_t>(LV_PART_MAIN) | static_cast<lv_style_selector_t>(LV_STATE_DEFAULT));
  }
}

// Configure a button as a read-only sensor card (non-clickable, shows value + unit)
inline void setup_sensor_card(BtnSlot &s, const ParsedCfg &p,
                              bool has_sensor_color, uint32_t sensor_val) {
  if (has_sensor_color) {
    lv_obj_set_style_bg_color(s.btn, lv_color_hex(sensor_val),
      static_cast<lv_style_selector_t>(LV_PART_MAIN) | static_cast<lv_style_selector_t>(LV_STATE_DEFAULT));
  }
  lv_obj_clear_flag(s.btn, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_flag(s.icon_lbl, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(s.sensor_container, LV_OBJ_FLAG_HIDDEN);
  if (!p.unit.empty()) {
    lv_label_set_text(s.unit_lbl, p.unit.c_str());
  }
  if (!p.label.empty()) {
    lv_label_set_text(s.text_lbl, p.label.c_str());
  }
}

// Set icon and label on a toggle/push button based on its config
inline void setup_toggle_visual(BtnSlot &s, const ParsedCfg &p) {
  if (!p.entity.empty()) {
    if (!p.label.empty()) {
      lv_label_set_text(s.text_lbl, p.label.c_str());
    }
    const char* icon_cp = "\U000F0493";
    if (p.icon.empty() || p.icon == "Auto") {
      std::string domain = p.entity.substr(0, p.entity.find('.'));
      icon_cp = domain_default_icon(domain);
    } else {
      icon_cp = find_icon(p.icon.c_str());
    }
    lv_label_set_text(s.icon_lbl, icon_cp);

    if (!p.sensor.empty()) {
      if (!p.unit.empty()) {
        lv_label_set_text(s.unit_lbl, p.unit.c_str());
      }
    }
  } else {
    if (!p.label.empty()) {
      lv_label_set_text(s.text_lbl, p.label.c_str());
    }
    if (!p.icon.empty() && p.icon != "Auto") {
      lv_label_set_text(s.icon_lbl, find_icon(p.icon.c_str()));
    } else if (p.type == "push") {
      lv_label_set_text(s.icon_lbl, "\U000F0741");
      static const lv_style_prop_t push_props[] = {LV_STYLE_BG_COLOR, LV_STYLE_PROP_INV};
      static lv_style_transition_dsc_t push_trans;
      static bool push_trans_inited = false;
      if (!push_trans_inited) {
        lv_style_transition_dsc_init(&push_trans, push_props, lv_anim_path_ease_out, 400, 0, NULL);
        push_trans_inited = true;
      }
      lv_obj_set_style_transition(s.btn, &push_trans,
        static_cast<lv_style_selector_t>(LV_PART_MAIN) | LV_STATE_DEFAULT);
    }
  }
}

// ── Home Assistant subscriptions ──────────────────────────────────────

// Subscribe to a HA sensor entity and update an LVGL label with its value
inline void subscribe_sensor_value(lv_obj_t *sensor_lbl, const std::string &sensor_id) {
  esphome::api::global_api_server->subscribe_home_assistant_state(
    sensor_id, {},
    std::function<void(const std::string &)>([sensor_lbl](const std::string &state) {
      char *end;
      float val = strtof(state.c_str(), &end);
      if (end != state.c_str()) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.0f", val);
        lv_label_set_text(sensor_lbl, buf);
      } else {
        lv_label_set_text(sensor_lbl, state.c_str());
      }
    })
  );
}

// Subscribe to an entity's friendly_name attribute and use it as the button label
inline void subscribe_friendly_name(lv_obj_t *text_lbl, const std::string &entity_id) {
  esphome::api::global_api_server->subscribe_home_assistant_state(
    entity_id, std::string("friendly_name"),
    std::function<void(const std::string &)>([text_lbl](const std::string &name) {
      lv_label_set_text(text_lbl, name.c_str());
    })
  );
}

// Subscribe to a toggle entity's state; updates checked visual, icon swap, sensor overlay
inline void subscribe_toggle_state(lv_obj_t *btn_ptr, lv_obj_t *icon_lbl,
                                   lv_obj_t *sensor_ctr,
                                   bool *slot_has_sensor, bool *slot_has_icon_on,
                                   const char **slot_icon_off, const char **slot_icon_on,
                                   const std::string &entity_id) {
  esphome::api::global_api_server->subscribe_home_assistant_state(
    entity_id, {},
    std::function<void(const std::string &)>(
      [btn_ptr, icon_lbl, sensor_ctr, slot_has_sensor, slot_has_icon_on,
       slot_icon_off, slot_icon_on](const std::string &state) {
        bool on = is_entity_on(state);
        if (on) lv_obj_add_state(btn_ptr, LV_STATE_CHECKED);
        else lv_obj_clear_state(btn_ptr, LV_STATE_CHECKED);
        if (*slot_has_icon_on) {
          lv_label_set_text(icon_lbl, on ? *slot_icon_on : *slot_icon_off);
        } else if (*slot_has_sensor) {
          if (on) {
            lv_obj_add_flag(icon_lbl, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(sensor_ctr, LV_OBJ_FLAG_HIDDEN);
          } else {
            lv_obj_clear_flag(icon_lbl, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(sensor_ctr, LV_OBJ_FLAG_HIDDEN);
          }
        }
      })
  );
}

// ── Home Assistant actions ────────────────────────────────────────────

// Send homeassistant.toggle for a given entity_id
inline void send_toggle_action(const std::string &entity_id) {
  esphome::api::HomeassistantActionRequest req;
  req.service = decltype(req.service)("homeassistant.toggle");
  req.is_event = false;
  req.data.init(1);
  auto &kv = req.data.emplace_back();
  kv.key = decltype(kv.key)("entity_id");
  kv.value = decltype(kv.value)(entity_id.c_str());
  esphome::api::global_api_server->send_homeassistant_action(req);
}

// ── Slider card helpers ────────────────────────────────────────────────

inline bool is_cover_entity(const std::string &entity_id) {
  return entity_id.size() > 6 && entity_id.compare(0, 6, "cover.") == 0;
}

// Send HA action for a slider change: toggle (value<0), brightness, or cover position
inline void send_slider_action(const std::string &entity_id, int value) {
  esphome::api::HomeassistantActionRequest req;
  req.is_event = false;
  if (value < 0) {
    req.service = decltype(req.service)("homeassistant.toggle");
    req.data.init(1);
    auto &kv = req.data.emplace_back();
    kv.key = decltype(kv.key)("entity_id");
    kv.value = decltype(kv.value)(entity_id.c_str());
  } else if (is_cover_entity(entity_id)) {
    req.service = decltype(req.service)("cover.set_cover_position");
    req.data.init(2);
    auto &kv1 = req.data.emplace_back();
    kv1.key = decltype(kv1.key)("entity_id");
    kv1.value = decltype(kv1.value)(entity_id.c_str());
    auto &kv2 = req.data.emplace_back();
    kv2.key = decltype(kv2.key)("position");
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", value);
    kv2.value = decltype(kv2.value)(buf);
  } else if (value == 0) {
    req.service = decltype(req.service)("light.turn_off");
    req.data.init(1);
    auto &kv = req.data.emplace_back();
    kv.key = decltype(kv.key)("entity_id");
    kv.value = decltype(kv.value)(entity_id.c_str());
  } else {
    req.service = decltype(req.service)("light.turn_on");
    req.data.init(2);
    auto &kv1 = req.data.emplace_back();
    kv1.key = decltype(kv1.key)("entity_id");
    kv1.value = decltype(kv1.value)(entity_id.c_str());
    auto &kv2 = req.data.emplace_back();
    kv2.key = decltype(kv2.key)("brightness_pct");
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", value);
    kv2.value = decltype(kv2.value)(buf);
  }
  esphome::api::global_api_server->send_homeassistant_action(req);
}

// ── Button click dispatch ─────────────────────────────────────────────

// Handle a main-grid button press: dispatch push event, subpage nav,
// slider toggle, or entity toggle based on the config string.
inline void handle_button_click(const std::string &cfg, int slot_num,
                                lv_obj_t *btn_obj) {
  ParsedCfg p = parse_cfg(cfg);
  if (p.type == "sensor") return;
  if (p.type == "push") {
    std::string label = p.label;
    if (label.empty()) {
      char buf[16];
      snprintf(buf, sizeof(buf), "Push %d", slot_num);
      label = buf;
    }
    esphome::api::HomeassistantActionRequest req;
    req.service = decltype(req.service)("esphome.push_button_pressed");
    req.is_event = true;
    req.data.init(2);
    auto &kv1 = req.data.emplace_back();
    kv1.key = decltype(kv1.key)("label");
    kv1.value = decltype(kv1.value)(label.c_str());
    auto &kv2 = req.data.emplace_back();
    kv2.key = decltype(kv2.key)("slot");
    char slot_buf[8];
    snprintf(slot_buf, sizeof(slot_buf), "%d", slot_num);
    kv2.value = decltype(kv2.value)(slot_buf);
    esphome::api::global_api_server->send_homeassistant_action(req);
  } else if (p.type == "subpage") {
    lv_obj_t *sub_scr = (lv_obj_t *)lv_obj_get_user_data(btn_obj);
    if (sub_scr)
      lv_scr_load_anim(sub_scr, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
  } else if (p.type == "slider" || p.type == "cover") {
    if (!p.entity.empty()) send_slider_action(p.entity, -1);
  } else {
    if (!p.entity.empty()) send_toggle_action(p.entity);
  }
}

// ── Slider widgets ───────────────────────────────────────────────────

// Context attached to each LVGL slider via user_data
struct SliderCtx {
  std::string entity_id;
  lv_obj_t *fill;
  bool horizontal;
  bool inverted;
  lv_coord_t radius;
};

// Resize the colored fill overlay to reflect the current slider percentage
inline void slider_update_fill(lv_obj_t *fill, lv_obj_t *btn, int pct, bool horizontal, bool inverted, lv_coord_t r) {
  lv_coord_t bw = lv_obj_get_width(btn);
  lv_coord_t bh = lv_obj_get_height(btn);
  lv_obj_set_style_radius(fill, r, LV_PART_MAIN);
  if (horizontal) {
    lv_coord_t w = (lv_coord_t)((int32_t)bw * pct / 100);
    lv_obj_set_size(fill, w, bh);
    lv_obj_align(fill, inverted ? LV_ALIGN_RIGHT_MID : LV_ALIGN_LEFT_MID, 0, 0);
  } else {
    lv_coord_t h = (lv_coord_t)((int32_t)bh * pct / 100);
    lv_obj_set_size(fill, bw, h);
    lv_obj_align(fill, inverted ? LV_ALIGN_TOP_MID : LV_ALIGN_BOTTOM_MID, 0, 0);
  }
}

// Create an invisible LVGL slider with a colored fill overlay inside a button
inline lv_obj_t *setup_slider_widget(lv_obj_t *btn, uint32_t on_color, bool horizontal) {
  lv_obj_set_style_pad_all(btn, 0,
    static_cast<lv_style_selector_t>(LV_PART_MAIN));
  lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICKABLE);

  lv_obj_t *fill = lv_obj_create(btn);
  lv_obj_set_size(fill, 0, 0);
  lv_obj_set_style_bg_color(fill, lv_color_hex(on_color), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(fill, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(fill, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(fill, 0, LV_PART_MAIN);
  lv_obj_clear_flag(fill, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *slider = lv_slider_create(btn);
  lv_slider_set_range(slider, 0, 100);
  lv_slider_set_value(slider, 0, LV_ANIM_OFF);
  lv_obj_update_layout(btn);
  lv_coord_t bw = lv_obj_get_width(btn);
  lv_coord_t bh = lv_obj_get_height(btn);
  if (horizontal) {
    lv_obj_set_size(slider, bw, bh >= bw ? bw - 1 : bh);
  } else {
    lv_obj_set_size(slider, bw >= bh ? bh - 1 : bw, bh);
  }
  lv_obj_align(slider, LV_ALIGN_CENTER, 0, 0);

  lv_obj_set_style_bg_opa(slider, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(slider, LV_OPA_TRANSP,
    static_cast<lv_style_selector_t>(LV_PART_INDICATOR));
  lv_obj_set_style_bg_opa(slider, LV_OPA_TRANSP,
    static_cast<lv_style_selector_t>(LV_PART_KNOB));
  lv_obj_set_style_border_width(slider, 0,
    static_cast<lv_style_selector_t>(LV_PART_KNOB));
  lv_obj_set_style_shadow_width(slider, 0,
    static_cast<lv_style_selector_t>(LV_PART_KNOB));
  lv_obj_set_style_pad_all(slider, 0,
    static_cast<lv_style_selector_t>(LV_PART_KNOB));
  lv_obj_set_style_width(slider, 0,
    static_cast<lv_style_selector_t>(LV_PART_KNOB));
  lv_obj_set_style_height(slider, 0,
    static_cast<lv_style_selector_t>(LV_PART_KNOB));

  lv_obj_move_to_index(fill, 0);
  lv_obj_move_to_index(slider, 1);

  return slider;
}

// Full slider button setup: visual + event handlers + HA action on release
inline void setup_slider_visual(BtnSlot &s, const ParsedCfg &p, uint32_t on_color) {
  setup_toggle_visual(s, p);

  bool horizontal = p.sensor == "h";
  lv_obj_t *slider = setup_slider_widget(s.btn, on_color, horizontal);
  lv_coord_t pad = lv_obj_get_style_radius(s.btn, LV_PART_MAIN) + 4;
  lv_obj_align(s.icon_lbl, LV_ALIGN_TOP_LEFT, pad, pad);
  lv_obj_align(s.text_lbl, LV_ALIGN_BOTTOM_LEFT, pad, -pad);
  lv_obj_set_user_data(s.sensor_container, (void *)slider);

  lv_obj_t *fill = lv_obj_get_child(s.btn, 0);
  // Intentionally leaked -- lives for the lifetime of the display
  SliderCtx *ctx = new SliderCtx();
  ctx->entity_id = p.entity;
  ctx->fill = fill;
  ctx->horizontal = horizontal;
  ctx->inverted = is_cover_entity(p.entity);
  ctx->radius = lv_obj_get_style_radius(s.btn, LV_PART_MAIN);
  lv_obj_set_user_data(slider, (void *)ctx);

  lv_obj_add_event_cb(slider, [](lv_event_t *e) {
    lv_obj_t *sl = lv_event_get_target(e);
    SliderCtx *c = (SliderCtx *)lv_obj_get_user_data(sl);
    if (!c) return;
    int val = lv_slider_get_value(sl);
    int fill_val = c->inverted ? 100 - val : val;
    slider_update_fill(c->fill, lv_obj_get_parent(sl), fill_val, c->horizontal, c->inverted, c->radius);
  }, LV_EVENT_VALUE_CHANGED, nullptr);

  lv_obj_add_event_cb(slider, [](lv_event_t *e) {
    lv_obj_t *sl = lv_event_get_target(e);
    SliderCtx *c = (SliderCtx *)lv_obj_get_user_data(sl);
    if (c && !c->entity_id.empty()) {
      int val = lv_slider_get_value(sl);
      send_slider_action(c->entity_id, val);
    }
  }, LV_EVENT_RELEASED, nullptr);
}

// Subscribe to HA state for a slider entity (light brightness or cover position)
inline void subscribe_slider_state(lv_obj_t *btn_ptr, lv_obj_t *icon_lbl,
                                  lv_obj_t *slider,
                                  bool has_icon_on,
                                  const char *icon_off, const char *icon_on,
                                  const std::string &entity_id) {
  SliderCtx *sctx = (SliderCtx *)lv_obj_get_user_data(slider);
  lv_obj_t *fill = sctx ? sctx->fill : nullptr;
  bool horiz = sctx ? sctx->horizontal : false;
  bool inv = sctx ? sctx->inverted : false;
  lv_coord_t rad = sctx ? sctx->radius : 0;
  bool is_cover = is_cover_entity(entity_id);
  esphome::api::global_api_server->subscribe_home_assistant_state(
    entity_id, {},
    std::function<void(const std::string &)>(
      [slider, btn_ptr, fill, horiz, inv, rad, icon_lbl, has_icon_on, icon_off, icon_on, is_cover](const std::string &state) {
        bool on = is_entity_on(state);
        if (!on) {
          lv_slider_set_value(slider, 0, LV_ANIM_OFF);
          if (fill) slider_update_fill(fill, btn_ptr, inv ? 100 : 0, horiz, inv, rad);
        }
        if (has_icon_on && !is_cover) {
          lv_label_set_text(icon_lbl, on ? icon_on : icon_off);
        }
      })
  );
  if (is_cover) {
    esphome::api::global_api_server->subscribe_home_assistant_state(
      entity_id, std::string("current_position"),
      std::function<void(const std::string &)>(
        [slider, btn_ptr, fill, horiz, inv, rad, icon_lbl, has_icon_on, icon_off, icon_on](const std::string &val) {
          char *end;
          float pos = strtof(val.c_str(), &end);
          if (end != val.c_str()) {
            int pct = (int)(pos + 0.5f);
            if (pct < 0) pct = 0;
            if (pct > 100) pct = 100;
            lv_slider_set_value(slider, pct, LV_ANIM_OFF);
            int fill_pct = inv ? 100 - pct : pct;
            if (fill) slider_update_fill(fill, btn_ptr, fill_pct, horiz, inv, rad);
            if (has_icon_on) {
              lv_label_set_text(icon_lbl, pct < 100 ? icon_on : icon_off);
            }
          }
        })
    );
  } else {
    esphome::api::global_api_server->subscribe_home_assistant_state(
      entity_id, std::string("brightness"),
      std::function<void(const std::string &)>(
        [slider, btn_ptr, fill, horiz, inv, rad](const std::string &val) {
          char *end;
          float bri = strtof(val.c_str(), &end);
          if (end != val.c_str()) {
            int pct = (int)((bri * 100.0f + 127.0f) / 255.0f);
            if (pct < 1) pct = 1;
            if (pct > 100) pct = 100;
            lv_slider_set_value(slider, pct, LV_ANIM_OFF);
            int fill_pct = inv ? 100 - pct : pct;
            if (fill) slider_update_fill(fill, btn_ptr, fill_pct, horiz, inv, rad);
          }
        })
    );
  }
}

// ── Subpage helpers ───────────────────────────────────────────────────

// Button definition parsed from a subpage config (pipe+colon delimited)
struct SubpageBtn {
  std::string entity;
  std::string label;
  std::string icon;
  std::string icon_on;
  std::string sensor;  // sensor entity for toggle; orientation "h"|"" for slider/cover
  std::string unit;
  std::string type;
};

// Create a slider button inside a subpage screen (reuses main grid slider logic)
inline lv_obj_t *setup_subpage_slider(lv_obj_t *btn, lv_obj_t *icon_lbl, lv_obj_t *text_lbl,
                                       const SubpageBtn &sb, uint32_t on_color, lv_coord_t radius) {
  if (!sb.label.empty()) lv_label_set_text(text_lbl, sb.label.c_str());
  else subscribe_friendly_name(text_lbl, sb.entity);

  bool horiz = sb.sensor == "h";
  lv_obj_t *sl = setup_slider_widget(btn, on_color, horiz);
  lv_coord_t pad = radius + 4;
  lv_obj_align(icon_lbl, LV_ALIGN_TOP_LEFT, pad, pad);
  lv_obj_align(text_lbl, LV_ALIGN_BOTTOM_LEFT, pad, -pad);

  lv_obj_t *fill = lv_obj_get_child(btn, 0);
  // Intentionally leaked -- lives for the lifetime of the display
  SliderCtx *ctx = new SliderCtx();
  ctx->entity_id = sb.entity;
  ctx->fill = fill;
  ctx->horizontal = horiz;
  ctx->inverted = is_cover_entity(sb.entity);
  ctx->radius = radius;
  lv_obj_set_user_data(sl, (void *)ctx);

  lv_obj_add_event_cb(sl, [](lv_event_t *e) {
    lv_obj_t *s = lv_event_get_target(e);
    SliderCtx *c = (SliderCtx *)lv_obj_get_user_data(s);
    if (!c) return;
    int val = lv_slider_get_value(s);
    int fv = c->inverted ? 100 - val : val;
    slider_update_fill(c->fill, lv_obj_get_parent(s), fv, c->horizontal, c->inverted, c->radius);
  }, LV_EVENT_VALUE_CHANGED, nullptr);

  lv_obj_add_event_cb(sl, [](lv_event_t *e) {
    lv_obj_t *s = lv_event_get_target(e);
    SliderCtx *c = (SliderCtx *)lv_obj_get_user_data(s);
    if (c && !c->entity_id.empty())
      send_slider_action(c->entity_id, lv_slider_get_value(s));
  }, LV_EVENT_RELEASED, nullptr);

  bool has_icon_on = !sb.icon_on.empty() && sb.icon_on != "Auto";
  const char *sl_icon_on = has_icon_on ? find_icon(sb.icon_on.c_str()) : nullptr;
  const char *sl_icon_off = nullptr;
  if (has_icon_on) {
    sl_icon_off = (sb.icon.empty() || sb.icon == "Auto")
      ? domain_default_icon(sb.entity.substr(0, sb.entity.find('.')))
      : find_icon(sb.icon.c_str());
  }
  subscribe_slider_state(btn, icon_lbl, sl, has_icon_on, sl_icon_off, sl_icon_on, sb.entity);

  // Intentionally leaked -- lives for the lifetime of the display
  std::string *eid = new std::string(sb.entity);
  lv_obj_add_event_cb(btn, [](lv_event_t *e) {
    std::string *en = (std::string *)lv_event_get_user_data(e);
    if (en && !en->empty()) send_slider_action(*en, -1);
  }, LV_EVENT_CLICKED, eid);

  return sl;
}

// Parse "order|entity:label:icon:...|entity:label:..." into a vector of SubpageBtns
inline std::vector<SubpageBtn> parse_subpage_config(const std::string &sp_cfg) {
  std::vector<SubpageBtn> btns;
  if (sp_cfg.empty()) return btns;

  std::vector<std::string> pipes;
  size_t ps = 0;
  while (ps <= sp_cfg.length()) {
    size_t pe = sp_cfg.find('|', ps);
    if (pe == std::string::npos) pe = sp_cfg.length();
    pipes.push_back(sp_cfg.substr(ps, pe - ps));
    ps = pe + 1;
  }
  if (pipes.size() < 2) return btns;

  for (size_t pi = 1; pi < pipes.size(); pi++) {
    std::vector<std::string> flds;
    size_t fs = 0;
    while (fs <= pipes[pi].length()) {
      size_t fe = pipes[pi].find(':', fs);
      if (fe == std::string::npos) fe = pipes[pi].length();
      flds.push_back(pipes[pi].substr(fs, fe - fs));
      fs = fe + 1;
    }
    std::string e = flds.size() > 0 ? flds[0] : "";
    std::string l = flds.size() > 1 ? flds[1] : "";
    std::string ic = flds.size() > 2 ? flds[2] : "Auto";
    if (ic.empty()) ic = "Auto";
    std::string io = flds.size() > 3 ? flds[3] : "Auto";
    if (io.empty()) io = "Auto";
    std::string sn = flds.size() > 4 ? flds[4] : "";
    std::string un = flds.size() > 5 ? flds[5] : "";
    std::string tp = flds.size() > 6 ? flds[6] : "";
    btns.push_back({e, l, ic, io, sn, un, tp});
  }
  return btns;
}

// Extract the order string (everything before the first pipe) from subpage config
inline std::string get_subpage_order(const std::string &sp_cfg) {
  if (sp_cfg.empty()) return "";
  size_t pe = sp_cfg.find('|');
  if (pe == std::string::npos) return sp_cfg;
  return sp_cfg.substr(0, pe);
}

// Subpage grid layout with support for a back button token ("B")
struct SubpageOrder {
  int positions[25] = {};
  bool is_double[25] = {};
  bool is_wide[25] = {};
  int back_pos = 0;
  bool back_dbl = false;
  bool back_wide = false;
  bool has_back_token = false;
};

// Parse subpage order CSV; "B"/"Bd"/"Bw"/"Bb" tokens mark the back button position
inline void parse_subpage_order(const std::string &order_str, int num_slots, int num_btns,
                                SubpageOrder &result) {
  if (order_str.empty()) return;
  size_t gp2 = 0, st2 = 0;
  while (st2 <= order_str.length() && gp2 < (size_t)num_slots) {
    size_t cm = order_str.find(',', st2);
    if (cm == std::string::npos) cm = order_str.length();
    if (cm > st2) {
      std::string tk = order_str.substr(st2, cm - st2);
      if (tk == "B" || tk == "Bd" || tk == "Bw" || tk == "Bb") {
        result.back_pos = gp2;
        result.back_dbl = (tk == "Bd" || tk == "Bb");
        result.back_wide = (tk == "Bw" || tk == "Bb");
        result.has_back_token = true;
      } else {
        bool d = !tk.empty() && tk.back() == 'd';
        bool w = !tk.empty() && tk.back() == 'w';
        bool bg = !tk.empty() && tk.back() == 'b';
        if (bg) { d = true; w = true; }
        if (d || w) tk.pop_back();
        int v = atoi(tk.c_str());
        if (v >= 1 && v <= num_btns) {
          result.positions[gp2] = v;
          result.is_double[v - 1] = d;
          result.is_wide[v - 1] = w;
        }
      }
    }
    gp2++;
    st2 = cm + 1;
  }
}
