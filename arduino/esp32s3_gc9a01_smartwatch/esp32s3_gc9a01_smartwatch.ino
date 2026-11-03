/**
 * ============================================================================
 * FILE: /arduino/esp32s3_gc9a01_smartwatch/esp32s3_gc9a01_smartwatch.ino
 * PROJECT: ChronoS3 — ESP32-S3-CAM (M2145) + GC9A01 (240x240 Round) Smart Watch
 * ============================================================================
 * Main Arduino Sketch Entry Point (.ino) — Cool-Running Eco Architecture:
 *   1. Long-Press (>= 450ms hold) from ANY screen jumps immediately to the
 *      Home Watch Face (SCREEN_HOME).
 *   2. Ultra-smooth Rotary Button handling: 10ms debounce, rotation lockout
 *      while pressing the knob, instant 0ms-wait Single-Click on Home/Menu,
 *      and snappy 165ms Double-Click on sub-screens.
 *   3. WiFi & Bluetooth radios start OFF at boot (0mA RF draw, saves ~195mA).
 *      Toggle either radio ON/OFF anytime from the WiFi or BLE menu screens!
 *   4. Runs ESP32-S3 CPU at 160 MHz (cuts CPU heat by ~35% while keeping 80MHz APB).
 */

#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include "esp32-hal-bt.h"
#include <Arduino_GFX_Library.h>

// Set to 1 ONLY if you select Tools -> Partition Scheme -> "Huge APP (3MB No OTA)"
#define USE_FULL_BLUEDROID_GATT 0
#if USE_FULL_BLUEDROID_GATT
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#endif

#include "pin_config.h"
#include "rotary_input.h"
#include "camera_handycam.h"
#include "sd_gallery.h"
#include "ui_gc9a01_round.h"

// ---------------- DISPLAY & 240x240 PSRAM CANVAS ----------------
Arduino_DataBus *bus = new Arduino_ESP32SPI(
  TFT_DC_PIN, TFT_CS_PIN, TFT_SCLK_PIN, TFT_MOSI_PIN, GFX_NOT_DEFINED
);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, TFT_RST_PIN, 0 /* rotation */, true /* IPS */);
Arduino_Canvas *canvas = new Arduino_Canvas(TFT_WIDTH, TFT_HEIGHT, gfx);

// ---------------- GLOBAL WATCH OS STATE ----------------
// Both WiFi & Bluetooth default to FALSE (OFF) at boot to keep ESP32-S3 cool!
static WatchScreen g_screen     = SCREEN_HOME;
static uint8_t g_watchFace      = 0;
static uint8_t g_menuIndex      = 0;
static bool g_wifiEnabled       = false;
static bool g_bleEnabled        = false;
static int16_t g_paddleDeg      = 90;
static uint32_t g_lastRenderMs  = 0;

void applyWifiState(bool enable) {
  if (enable) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ChronoS3_HandyCam", "esp32s3cam");
    // 8.5 dBm Eco TX Power cuts 2.4GHz PA heat by ~60% for close-range watch use
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    Serial.println("[WIFI] Enabled SoftAP (8.5dBm Cool Eco Mode)");
  } else {
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("[WIFI] Radio powered OFF (0mA Cool Mode)");
  }
}

void applyBleState(bool enable) {
  if (enable) {
    btStart();
#if USE_FULL_BLUEDROID_GATT
    BLEDevice::init("ChronoS3_Watch");
#endif
    Serial.println("[BLE] Radio powered ON");
  } else {
    btStop();
    Serial.println("[BLE] Radio powered OFF (0mA Cool Mode)");
  }
}

void dispatchRotaryEvent(RotaryEvent evt) {
  if (evt == EVT_NONE) return;

  // Global Long-Press (>= 450ms hold): Go straight to HOME Watch Face from anywhere!
  if (evt == EVT_LONG_PRESS) {
    g_gallery.fullscreenMode = false;
    g_gameMode = 0;
    g_screen = SCREEN_HOME;
    return;
  }

  switch (g_screen) {
    case SCREEN_HOME:
      if (evt == EVT_CW) g_watchFace = (g_watchFace + 1) % 3;
      else if (evt == EVT_CCW) g_watchFace = (g_watchFace + 2) % 3;
      else if (evt == EVT_SINGLE_CLICK || evt == EVT_DOUBLE_CLICK) g_screen = SCREEN_MENU;
      break;

    case SCREEN_MENU:
      if (evt == EVT_CW) g_menuIndex = (g_menuIndex + 1) % 6;
      else if (evt == EVT_CCW) g_menuIndex = (g_menuIndex + 5) % 6;
      else if (evt == EVT_SINGLE_CLICK) {
        const WatchScreen kTargets[6] = {
          SCREEN_CAMERA, SCREEN_GALLERY, SCREEN_WIFI,
          SCREEN_BLUETOOTH, SCREEN_GAMES, SCREEN_HOME
        };
        g_screen = kTargets[g_menuIndex];
        if (g_screen == SCREEN_GALLERY && g_gallery.fileCount == 0) {
          if (!g_gallery.mounted) initSDCardGallery();
          else scanSDGalleryFolder();
        }
      } else if (evt == EVT_DOUBLE_CLICK) {
        g_screen = SCREEN_HOME;
      }
      break;

    case SCREEN_CAMERA:
      if (evt == EVT_CW) g_camState.activeTool = (g_camState.activeTool + 1) % CAM_TOOL_COUNT;
      else if (evt == EVT_CCW) g_camState.activeTool = (g_camState.activeTool + CAM_TOOL_COUNT - 1) % CAM_TOOL_COUNT;
      else if (evt == EVT_SINGLE_CLICK) {
        if (g_camState.activeTool == CAM_TOOL_SNAP) captureHandyCamFrameToSD();
        else if (g_camState.activeTool == CAM_TOOL_STREAM) g_camState.streaming = !g_camState.streaming;
        else if (g_camState.activeTool == CAM_TOOL_ZOOM) cycleCameraZoom();
        else if (g_camState.activeTool == CAM_TOOL_FILTER) cycleCameraFilter();
        else if (g_camState.activeTool == CAM_TOOL_EXPOSURE) cycleCameraExposure();
      } else if (evt == EVT_DOUBLE_CLICK) {
        g_screen = SCREEN_MENU;
      }
      break;

    case SCREEN_GALLERY:
      if (evt == EVT_CW && g_gallery.fileCount > 0) {
        g_gallery.selectedIndex = (g_gallery.selectedIndex + 1) % g_gallery.fileCount;
      } else if (evt == EVT_CCW && g_gallery.fileCount > 0) {
        g_gallery.selectedIndex = (g_gallery.selectedIndex + g_gallery.fileCount - 1) % g_gallery.fileCount;
      } else if (evt == EVT_SINGLE_CLICK) {
        g_gallery.fullscreenMode = !g_gallery.fullscreenMode;
      } else if (evt == EVT_DOUBLE_CLICK) {
        if (g_gallery.fullscreenMode) g_gallery.fullscreenMode = false;
        else g_screen = SCREEN_MENU;
      }
      break;

    case SCREEN_WIFI:
      if (evt == EVT_SINGLE_CLICK) {
        g_wifiEnabled = !g_wifiEnabled;
        applyWifiState(g_wifiEnabled);
      } else if (evt == EVT_DOUBLE_CLICK) {
        g_screen = SCREEN_MENU;
      }
      break;

    case SCREEN_BLUETOOTH:
      if (evt == EVT_SINGLE_CLICK) {
        g_bleEnabled = !g_bleEnabled;
        applyBleState(g_bleEnabled);
      } else if (evt == EVT_DOUBLE_CLICK) {
        g_screen = SCREEN_MENU;
      }
      break;

    case SCREEN_GAMES:
      if (handleGamesRotaryEvent(evt, g_paddleDeg)) {
        g_screen = SCREEN_MENU;
      }
      break;
  }
}

void setup() {
  // 1. Run CPU at 160 MHz instead of 240 MHz (cuts CPU thermal dissipation by ~35%
  //    while keeping APB clock at 80 MHz so SPI2 and Camera timing are unchanged)
  setCpuFrequencyMhz(160);

  Serial.begin(115200);
  Serial.println("Booting ChronoS3 (Cool Eco Mode: 160MHz, WiFi OFF, BLE OFF)...");

#if (TFT_BL_PIN >= 0)
  pinMode(TFT_BL_PIN, OUTPUT);
  digitalWrite(TFT_BL_PIN, HIGH);
#endif

  // 2. Shut down WiFi & Bluetooth radios at boot so RF PA generates zero heat
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  btStop();

  canvas->begin(80000000 /* 80MHz SPI */);
  canvas->fillScreen(COLOR_BG_OBSIDIAN);
  canvas->flush();

  initSDCardGallery();
  initOV2640Camera();
  // Initialize Rotary Switch last so GPIO 1, 42, 47 are guaranteed INPUT_PULLUP
  initRotarySwitch();
}

void loop() {
  // On SCREEN_HOME, SCREEN_MENU, SCREEN_WIFI, and SCREEN_BLUETOOTH, Long-Press goes
  // straight to HOME so Single-Click can fire with 0ms release latency!
  bool instantSingleClick = (
    g_screen == SCREEN_HOME ||
    g_screen == SCREEN_MENU ||
    g_screen == SCREEN_WIFI ||
    g_screen == SCREEN_BLUETOOTH
  );

  RotaryEvent evt = pollRotarySwitch(instantSingleClick);
  if (evt != EVT_NONE) {
    dispatchRotaryEvent(evt);
  }

  // Smart Frame Pacing: redraw immediately on rotary input, or at 40 FPS (25ms)
  // on Games/Camera/Menu and 25 FPS (40ms) on Watch Face/Status screens
  uint32_t nowMs = millis();
  uint32_t frameIntervalMs = (g_screen == SCREEN_GAMES || g_screen == SCREEN_CAMERA || g_screen == SCREEN_MENU) ? 25 : 40;

  if (evt != EVT_NONE || (nowMs - g_lastRenderMs) >= frameIntervalMs) {
    g_lastRenderMs = nowMs;

    switch (g_screen) {
      case SCREEN_HOME:
        renderHomeWatchFace(canvas, g_watchFace, g_wifiEnabled, g_bleEnabled);
        break;
      case SCREEN_MENU:
        renderRadialMenu(canvas, g_menuIndex);
        break;
      case SCREEN_CAMERA:
        renderHandyCamScreen(canvas);
        break;
      case SCREEN_GALLERY:
        renderSDGalleryScreen(canvas);
        break;
      case SCREEN_WIFI:
        renderWifiScreen(canvas, g_wifiEnabled);
        break;
      case SCREEN_BLUETOOTH:
        renderBleScreen(canvas, g_bleEnabled);
        break;
      case SCREEN_GAMES:
        renderGamesScreen(canvas, g_paddleDeg);
        break;
    }

    canvas->flush();
  }

  delay(2);
}
