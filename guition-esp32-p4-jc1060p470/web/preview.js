(function () {
  var ICON_MAP = {
    Auto: "cog",
    Lightbulb: "lightbulb",
    "Power Plug": "power-plug",
    Fan: "fan",
    Lock: "lock",
    Garage: "garage",
    "Blinds Open": "blinds-horizontal",
    "Blinds Closed": "blinds-horizontal-closed",
    Thermometer: "thermometer",
    Speaker: "speaker",
    Television: "television",
    Camera: "camera",
    "Motion Sensor": "motion-sensor",
    Door: "door",
    Window: "window-open-variant",
    "Water Heater": "water-boiler",
    "Air Conditioner": "air-filter",
    Battery: "battery",
    "LED Strip": "led-strip",
    Power: "power",
  };

  var DOMAIN_ICONS = {
    light: "lightbulb",
    switch: "power-plug",
    fan: "fan",
    lock: "lock",
    cover: "blinds-horizontal",
    climate: "air-filter",
    media_player: "speaker",
    camera: "camera",
    binary_sensor: "motion-sensor",
  };

  var state = {
    entity: "",
    label: "",
    icon: "Auto",
    onColor: "FF8C00",
    offColor: "313131",
    checked: false,
  };

  var els = {};

  function init() {
    loadFonts();
    buildPreview();
    connectEvents();
  }

  function loadFonts() {
    var mdi = document.createElement("link");
    mdi.rel = "stylesheet";
    mdi.href =
      "https://cdn.jsdelivr.net/npm/@mdi/font@7.4.47/css/materialdesignicons.min.css";
    document.head.appendChild(mdi);

    var roboto = document.createElement("link");
    roboto.rel = "stylesheet";
    roboto.href =
      "https://fonts.googleapis.com/css2?family=Roboto:wght@300;400&display=swap";
    document.head.appendChild(roboto);
  }

  function buildPreview() {
    var wrap = document.createElement("div");
    wrap.className = "sp-wrap";
    wrap.innerHTML =
      '<div class="sp-screen">' +
      '<div class="sp-topbar">' +
      '<span class="sp-temp"></span>' +
      '<span class="sp-clock">--:--</span>' +
      "</div>" +
      '<div class="sp-main">' +
      '<div class="sp-btn">' +
      '<span class="sp-btn-icon mdi mdi-cog"></span>' +
      '<span class="sp-btn-label">Configure</span>' +
      "</div>" +
      "</div>" +
      "</div>" +
      '<div class="sp-hint">tap button to preview on/off state</div>';

    var app = document.querySelector("esp-app");
    if (app) {
      app.parentNode.insertBefore(wrap, app);
    } else {
      document.body.insertBefore(wrap, document.body.firstChild);
    }

    els.temp = wrap.querySelector(".sp-temp");
    els.clock = wrap.querySelector(".sp-clock");
    els.btn = wrap.querySelector(".sp-btn");
    els.icon = wrap.querySelector(".sp-btn-icon");
    els.label = wrap.querySelector(".sp-btn-label");

    els.btn.addEventListener("click", function () {
      state.checked = !state.checked;
      applyButtonColor();
    });

    applyButtonColor();
    updateClock();
    setInterval(updateClock, 30000);
  }

  function resolveIcon() {
    var sel = state.icon;
    if (sel === "Auto" && state.entity) {
      var domain = state.entity.split(".")[0];
      return DOMAIN_ICONS[domain] || "cog";
    }
    return ICON_MAP[sel] || "cog";
  }

  function applyButtonColor() {
    var hex = state.checked ? state.onColor : state.offColor;
    if (hex.length === 6) {
      els.btn.style.backgroundColor = "#" + hex;
    }
  }

  function applyIcon() {
    var name = resolveIcon();
    els.icon.className = "sp-btn-icon mdi mdi-" + name;
  }

  function applyLabel() {
    els.label.textContent = state.label || "Configure";
  }

  function updateClock() {
    var now = new Date();
    var h = String(now.getHours()).padStart(2, "0");
    var m = String(now.getMinutes()).padStart(2, "0");
    els.clock.textContent = h + ":" + m;
  }

  function connectEvents() {
    var source = new EventSource("/events");

    source.addEventListener("state", function (e) {
      var d;
      try {
        d = JSON.parse(e.data);
      } catch (_) {
        return;
      }

      var id = d.id;
      var val = d.state != null ? String(d.state) : "";

      switch (id) {
        case "text-button_1_entity":
          state.entity = val;
          applyIcon();
          break;

        case "text-button_1_label":
          state.label = val;
          applyLabel();
          break;

        case "select-button_1_icon":
          state.icon = val;
          applyIcon();
          break;

        case "text-button_on_color":
          state.onColor = val;
          applyButtonColor();
          break;

        case "text-button_off_color":
          state.offColor = val;
          applyButtonColor();
          break;

        case "switch-indoor_temp_enable":
        case "switch-outdoor_temp_enable":
          updateTempVisibility(d);
          break;
      }
    });
  }

  function updateTempVisibility(d) {
    if (
      d.id === "switch-indoor_temp_enable" ||
      d.id === "switch-outdoor_temp_enable"
    ) {
      var on = d.value === true || d.state === "ON";
      if (d.id === "switch-indoor_temp_enable") state._indoorOn = on;
      if (d.id === "switch-outdoor_temp_enable") state._outdoorOn = on;
    }
    var show = state._indoorOn || state._outdoorOn;
    els.temp.className = "sp-temp" + (show ? " sp-visible" : "");
    if (!els.temp.textContent) els.temp.textContent = "-\u00B0 / -\u00B0";
  }

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", init);
  } else {
    init();
  }
})();
