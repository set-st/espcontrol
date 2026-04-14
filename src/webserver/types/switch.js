// Default button type: HA entity toggle (on/off switch)
registerButtonType("", {
  label: "Switch",
  allowInSubpage: true,
  renderPreview: function (b, helpers) {
    var label = b.label || b.entity || "Configure";
    return {
      labelHtml:
        '<span class="sp-btn-label-row"><span class="sp-btn-label">' + helpers.escHtml(label) + '</span>' +
        '<span class="sp-type-badge mdi mdi-toggle-switch-variant-off"></span></span>',
    };
  },
});
