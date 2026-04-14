registerButtonType("slider", {
  label: "Slider",
  allowInSubpage: true,
  labelPlaceholder: "e.g. Living Room",
  onSelect: function (b) {
    b.sensor = ""; b.unit = ""; b.icon_on = "Auto";
    b.icon = "Auto";
  },
  renderSettings: function (panel, b, slot, helpers) {
    var ef = document.createElement("div");
    ef.className = "sp-field";
    ef.appendChild(helpers.fieldLabel("Entity ID", helpers.idPrefix + "entity"));
    var entityInp = helpers.textInput(helpers.idPrefix + "entity", b.entity, "e.g. light.living_room or cover.blinds");
    ef.appendChild(entityInp);
    panel.appendChild(ef);
    helpers.bindField(entityInp, "entity", true);

    panel.appendChild(helpers.makeIconPicker(
      helpers.idPrefix + "icon-picker", helpers.idPrefix + "icon",
      b.icon || "Auto", function (opt) {
        b.icon = opt;
        helpers.saveField("icon", opt);
        renderPreview();
      }
    ));

    var isHoriz = b.sensor === "h";
    var of = document.createElement("div");
    of.className = "sp-field";
    of.appendChild(helpers.fieldLabel("Direction"));
    var seg = document.createElement("div");
    seg.className = "sp-segment";
    var btnV = document.createElement("button");
    btnV.type = "button"; btnV.tabIndex = -1;
    btnV.textContent = "Vertical";
    if (!isHoriz) btnV.classList.add("active");
    var btnH = document.createElement("button");
    btnH.type = "button"; btnH.tabIndex = -1;
    btnH.textContent = "Horizontal";
    if (isHoriz) btnH.classList.add("active");
    seg.appendChild(btnV);
    seg.appendChild(btnH);
    of.appendChild(seg);
    panel.appendChild(of);

    btnV.addEventListener("click", function () {
      btnV.classList.add("active"); btnH.classList.remove("active");
      b.sensor = "";
      helpers.saveField("sensor", "");
      renderPreview();
    });
    btnH.addEventListener("click", function () {
      btnH.classList.add("active"); btnV.classList.remove("active");
      b.sensor = "h";
      helpers.saveField("sensor", "h");
      renderPreview();
    });

    var hasIconOn = b.icon_on && b.icon_on !== "Auto";
    var iconOnToggle = helpers.toggleRow("Change Icon When On", helpers.idPrefix + "iconon-toggle", hasIconOn);
    panel.appendChild(iconOnToggle.row);

    var iconOnCond = condField();
    if (hasIconOn) iconOnCond.classList.add("sp-visible");

    var iconOnSection = document.createElement("div");
    iconOnSection.className = "sp-field";
    iconOnSection.appendChild(helpers.fieldLabel("Icon When On", helpers.idPrefix + "icon-on"));
    var iconOnVal = hasIconOn ? b.icon_on : "Auto";
    var iconOnPicker = document.createElement("div");
    iconOnPicker.className = "sp-icon-picker";
    iconOnPicker.id = helpers.idPrefix + "icon-on-picker";
    iconOnPicker.innerHTML =
      '<span class="sp-icon-picker-preview mdi mdi-' + iconSlug(iconOnVal) + '"></span>' +
      '<input class="sp-icon-picker-input" id="' + helpers.idPrefix + 'icon-on" type="text" ' +
      'placeholder="Search icons\u2026" value="' + escAttr(iconOnVal) + '" autocomplete="off">' +
      '<div class="sp-icon-dropdown"></div>';
    iconOnSection.appendChild(iconOnPicker);
    iconOnCond.appendChild(iconOnSection);

    initIconPicker(iconOnPicker, iconOnVal, function (opt) {
      b.icon_on = opt;
      helpers.saveField("icon_on", opt);
      renderPreview();
    });

    panel.appendChild(iconOnCond);

    iconOnToggle.input.addEventListener("change", function () {
      if (this.checked) {
        iconOnCond.classList.add("sp-visible");
      } else {
        b.icon_on = "Auto";
        helpers.saveField("icon_on", "Auto");
        iconOnCond.classList.remove("sp-visible");
        var ionPreview = iconOnPicker.querySelector(".sp-icon-picker-preview");
        if (ionPreview) ionPreview.className = "sp-icon-picker-preview mdi mdi-cog";
        var ionInput = iconOnPicker.querySelector(".sp-icon-picker-input");
        if (ionInput) ionInput.value = "Auto";
        renderPreview();
      }
    });
  },
  renderPreview: function (b, helpers) {
    var label = b.label || b.entity || "Slider";
    var autoIcon = (b.entity && b.entity.indexOf("cover.") === 0) ? "blinds-horizontal" : "lightbulb";
    var iconName = b.icon && b.icon !== "Auto" ? iconSlug(b.icon) : autoIcon;
    var horizClass = b.sensor === "h" ? " sp-slider-horiz" : "";
    return {
      iconHtml:
        '<span class="sp-btn-icon mdi mdi-' + iconName + '"></span>' +
        '<span class="sp-slider-preview' + horizClass + '"><span class="sp-slider-track">' +
          '<span class="sp-slider-fill"></span>' +
        '</span></span>',
      labelHtml:
        '<span class="sp-btn-label-row"><span class="sp-btn-label">' + helpers.escHtml(label) + '</span>' +
        '<span class="sp-type-badge mdi mdi-tune-vertical-variant"></span></span>',
    };
  },
});
