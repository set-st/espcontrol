#pragma once

// Internal implementation detail for button_grid.h. Include button_grid.h from device YAML.

struct SwitchConfirmationModalUi {
  lv_obj_t *overlay = nullptr;
  lv_obj_t *panel = nullptr;
  lv_obj_t *message_lbl = nullptr;
  lv_obj_t *cancel_btn = nullptr;
  lv_obj_t *confirm_btn = nullptr;
  lv_obj_t *btn_obj = nullptr;
  ParsedCfg cfg;
};

inline SwitchConfirmationModalUi &switch_confirmation_modal_ui() {
  static SwitchConfirmationModalUi ui;
  return ui;
}

inline const lv_font_t *&switch_confirmation_message_font_ref() {
  static const lv_font_t *font = nullptr;
  return font;
}

inline void set_switch_confirmation_message_font(const lv_font_t *font) {
  switch_confirmation_message_font_ref() = font;
}

inline const lv_font_t *switch_confirmation_message_font(const lv_font_t *fallback) {
  const lv_font_t *font = switch_confirmation_message_font_ref();
  return font ? font : fallback;
}

inline void switch_confirmation_hide_modal() {
  SwitchConfirmationModalUi &ui = switch_confirmation_modal_ui();
  if (ui.overlay) lv_obj_del(ui.overlay);
  ui = SwitchConfirmationModalUi();
}

inline lv_obj_t *switch_confirmation_create_text_button(
    lv_obj_t *parent,
    const std::string &text,
    lv_coord_t width,
    lv_coord_t height,
    lv_coord_t radius,
    uint32_t bg_color,
    const lv_font_t *font) {
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_size(btn, width, height);
  lv_obj_set_style_radius(btn, radius, LV_PART_MAIN);
  lv_obj_set_style_bg_color(btn, lv_color_hex(bg_color), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
  lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, text.c_str());
  lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
  lv_obj_set_width(label, width - 12);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  if (font) lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
  lv_obj_center(label);
  return btn;
}

inline void switch_confirmation_confirm() {
  SwitchConfirmationModalUi &ui = switch_confirmation_modal_ui();
  if (!ui.cfg.entity.empty()) send_turn_off_action(ui.cfg.entity);
  if (ui.btn_obj) lv_obj_clear_state(ui.btn_obj, LV_STATE_CHECKED);
  switch_confirmation_hide_modal();
}

inline void switch_confirmation_open_modal(const ParsedCfg &p, lv_obj_t *btn_obj) {
  if (p.entity.empty()) return;
  media_volume_hide_modal();
  climate_control_hide_modal();
  switch_confirmation_hide_modal();

  SwitchConfirmationModalUi &ui = switch_confirmation_modal_ui();
  ui.cfg = p;
  ui.btn_obj = btn_obj;

  ControlModalLayout layout = control_modal_calc_layout(100);
  lv_coord_t radius = control_modal_card_radius(btn_obj);
  lv_coord_t content_w = layout.panel_w - layout.inset * 2;
  if (content_w < 120) content_w = layout.panel_w;
  lv_coord_t button_gap = control_modal_scaled_px(12, layout.short_side);
  if (button_gap < 8) button_gap = 8;
  lv_coord_t button_h = control_modal_scaled_px(52, layout.short_side);
  if (button_h < 36) button_h = 36;
  lv_coord_t button_w = (content_w - button_gap) / 2;
  if (button_w < 56) button_w = content_w;

  const lv_font_t *button_font = btn_obj
    ? lv_obj_get_style_text_font(btn_obj, LV_PART_MAIN)
    : nullptr;
  const lv_font_t *message_font = switch_confirmation_message_font(button_font);

  ui.overlay = lv_obj_create(lv_layer_top());
  control_modal_style_overlay(ui.overlay);

  ui.panel = lv_obj_create(ui.overlay);
  control_modal_style_panel(ui.panel, DEFAULT_TERTIARY_COLOR, radius);
  control_modal_apply_panel_layout(ui.overlay, ui.panel, layout, radius);

  ui.message_lbl = lv_label_create(ui.panel);
  std::string message = switch_confirmation_message(p);
  lv_label_set_text(ui.message_lbl, message.c_str());
  lv_label_set_long_mode(ui.message_lbl, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(ui.message_lbl, content_w);
  lv_obj_set_style_text_color(ui.message_lbl, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_set_style_text_align(ui.message_lbl, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  if (message_font) lv_obj_set_style_text_font(ui.message_lbl, message_font, LV_PART_MAIN);
  lv_obj_align(ui.message_lbl, LV_ALIGN_CENTER, 0, -button_h / 2);

  ui.cancel_btn = switch_confirmation_create_text_button(
    ui.panel, switch_confirmation_no_text(p), button_w, button_h,
    button_h / 2, 0x454545, button_font);
  ui.confirm_btn = switch_confirmation_create_text_button(
    ui.panel, switch_confirmation_yes_text(p), button_w, button_h,
    button_h / 2, DEFAULT_SLIDER_COLOR, button_font);

  if (button_w == content_w) {
    lv_obj_align(ui.confirm_btn, LV_ALIGN_BOTTOM_MID, 0, -layout.inset);
    lv_obj_align(ui.cancel_btn, LV_ALIGN_BOTTOM_MID, 0, -(layout.inset + button_h + button_gap));
  } else {
    lv_obj_align(ui.cancel_btn, LV_ALIGN_BOTTOM_LEFT, layout.inset, -layout.inset);
    lv_obj_align(ui.confirm_btn, LV_ALIGN_BOTTOM_RIGHT, -layout.inset, -layout.inset);
  }

  lv_obj_add_event_cb(ui.cancel_btn, [](lv_event_t *) {
    switch_confirmation_hide_modal();
  }, LV_EVENT_CLICKED, nullptr);
  lv_obj_add_event_cb(ui.confirm_btn, [](lv_event_t *) {
    switch_confirmation_confirm();
  }, LV_EVENT_CLICKED, nullptr);

  lv_obj_move_foreground(ui.overlay);
}
