registerButtonType("push", {
  label: "Push Button",
  allowInSubpage: false,
  labelPlaceholder: "e.g. Doorbell",
  onSelect: function (b) {
    b.entity = ""; b.sensor = ""; b.unit = ""; b.icon_on = "Auto";
  },
  renderSettings: function (panel, b, slot, helpers) {
    panel.appendChild(helpers.makeIconPicker(
      helpers.idPrefix + "icon-picker", helpers.idPrefix + "icon",
      b.icon || "Auto", function (opt) {
        b.icon = opt;
        helpers.saveField("icon", opt);
        renderPreview();
      }
    ));
  },
});
