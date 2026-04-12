registerButtonType("weather", {
  label: "Weather",
  allowInSubpage: false,
  labelPlaceholder: "e.g. My Location",
  onSelect: function (b) {
    b.entity = "";
    b.sensor = "";
    b.unit = "";
    b.icon = "Auto";
    b.icon_on = "Auto";
  },
  renderSettings: function (panel, b, slot, helpers) {
    var hint = document.createElement("div");
    hint.className = "sp-hint";
    hint.textContent = "Set location in Settings \u2192 Weather";
    panel.appendChild(hint);
  },
  renderPreview: function (b, helpers) {
    var label = b.label || "Weather";
    return {
      iconHtml: '<span class="sp-btn-icon mdi mdi-weather-partly-cloudy"></span>',
      labelHtml: '<span class="sp-btn-label">' + helpers.escHtml(label) + '</span>',
    };
  },
});
