registerButtonType("radar", {
  label: "Radar",
  allowInSubpage: false,
  labelPlaceholder: "e.g. Local Radar",
  onSelect: function (b) {
    b.entity = ""; b.sensor = ""; b.unit = "";
    b.icon = "Radar"; b.icon_on = "Auto";
  },
  renderSettings: function (panel, b, slot, helpers) {
    var lf = document.createElement("div");
    lf.className = "sp-field";
    lf.appendChild(helpers.fieldLabel("Latitude", helpers.idPrefix + "lat"));
    var latInp = helpers.textInput(helpers.idPrefix + "lat", b.lat || "", "e.g. 40.71");
    lf.appendChild(latInp);
    panel.appendChild(lf);
    helpers.bindField(latInp, "lat", true);

    var lnf = document.createElement("div");
    lnf.className = "sp-field";
    lnf.appendChild(helpers.fieldLabel("Longitude", helpers.idPrefix + "lon"));
    var lonInp = helpers.textInput(helpers.idPrefix + "lon", b.lon || "", "e.g. -74.01");
    lnf.appendChild(lonInp);
    panel.appendChild(lnf);
    helpers.bindField(lonInp, "lon", true);

    var zf = document.createElement("div");
    zf.className = "sp-field";
    zf.appendChild(helpers.fieldLabel("Zoom (1\u20137)", helpers.idPrefix + "zoom"));
    var zoomInp = helpers.textInput(helpers.idPrefix + "zoom", b.zoom || "6", "6");
    zoomInp.className = "sp-input sp-input--narrow";
    zf.appendChild(zoomInp);
    panel.appendChild(zf);
    helpers.bindField(zoomInp, "zoom", true);
  },
  renderPreview: function (b, helpers) {
    var label = b.label || "Radar";
    var lat = b.lat || "";
    var lon = b.lon || "";
    var zoom = b.zoom || "6";
    return {
      iconHtml:
        '<div class="sp-radar-tile" data-lat="' + helpers.escHtml(lat) +
        '" data-lon="' + helpers.escHtml(lon) +
        '" data-zoom="' + helpers.escHtml(zoom) + '">' +
          '<span class="sp-radar-placeholder mdi mdi-radar"></span>' +
        '</div>',
      labelHtml:
        '<span class="sp-btn-label">' + helpers.escHtml(label) + '</span>',
    };
  },
});
