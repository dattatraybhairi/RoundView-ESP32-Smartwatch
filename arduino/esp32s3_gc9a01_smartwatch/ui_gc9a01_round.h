/**
 * ============================================================================
 * FILE: /arduino/esp32s3_gc9a01_smartwatch/ui_gc9a01_round.h
 * PROJECT: ChronoS3 — 240x240 Circular GC9A01 UI, Icons, Watch Faces & Games
 * ============================================================================
 * Features:
 *  - Detailed vector logos/icons inside all 6 Radial Menu items
 *  - Dynamic color-changing bezel & smoothly interpolating selection halo ring
 *  - 3 Animated Android WearOS-grade Watch Faces (Ultra Chrono with smooth
 *    sweeping second hand + activity arcs + live ECG sine wave, Cyber Orbital
 *    with counter-rotating satellite rings, and Radar Telemetry)
 *  - Smooth 60+ FPS Delta-Time 3-Game Rotary Arcade Suite (Orbit Breaker,
 *    Radar Defense, and Cyber Tunnel) with smooth angular paddle/turret/ship
 *    interpolation, motion trails, and color-changing Lobby cards!
 */

#ifndef UI_GC9A01_ROUND_H
#define UI_GC9A01_ROUND_H

#include <Arduino.h>
#include <WiFi.h>
#include <Arduino_GFX_Library.h>
#include <math.h>
#include "pin_config.h"
#include "rotary_input.h"
#include "camera_handycam.h"
#include "sd_gallery.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

static const char *kMenuLabels[6] = {
  "HandyCam", "SD Gallery", "WiFi RTSP", "BLE 5.0", "Games", "Watch Face"
};

static const char *kMenuSubtitles[6] = {
  "M2145 Live Stream", "/DCIM Photo Viewer", "2.4GHz SoftAP",
  "Bluetooth Radio", "Orbital Arcade", "WearOS Faces"
};

static const uint16_t kMenuColors[6] = {
  COLOR_CYAN_ACCENT, COLOR_EMERALD, 0x3C3F, COLOR_VIOLET, COLOR_AMBER, COLOR_ROSE
};

inline void polarToScreen(float radius, float angleDeg, int16_t &outX, int16_t &outY) {
  float rad = (angleDeg - 90.0f) * ((float)M_PI / 180.0f);
  outX = (int16_t)(TFT_CENTER_X + radius * cosf(rad));
  outY = (int16_t)(TFT_CENTER_Y + radius * sinf(rad));
}

inline float shortestAngleDiffDeg(float a, float b) {
  float d = fmodf(a - b + 540.0f, 360.0f) - 180.0f;
  return fabsf(d);
}

inline void drawCircularBezelTicks(Arduino_Canvas *cv, uint16_t ringColor = COLOR_CYAN_ACCENT) {
  cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 118, ringColor);
  cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 117, ringColor);
  for (int deg = 0; deg < 360; deg += 6) {
    bool major = (deg % 30 == 0);
    int16_t x1, y1, x2, y2;
    polarToScreen(major ? 110.0f : 114.0f, (float)deg, x1, y1);
    polarToScreen(117.0f, (float)deg, x2, y2);
    cv->drawLine(x1, y1, x2, y2, major ? COLOR_TEXT_WHITE : COLOR_TEXT_MUTED);
  }
}

/**
 * Draws a crisp, detailed vector icon/logo centered at (cx, cy) for each of the 6 Menu items:
 * 0: HandyCam (Camera logo with prism, lens rings & shutter LED)
 * 1: SD Gallery (Framed mountain landscape & sun photo logo)
 * 2: WiFi (Concentric dual-weight signal waves logo)
 * 3: Bluetooth (BLE Nordic rune logo)
 * 4: Games (Arcade Gamepad D-pad + action buttons logo)
 * 5: Watch Face (Chronograph clock face + hands logo)
 */
inline void drawMenuLogo(Arduino_Canvas *cv, uint8_t iconId, int16_t cx, int16_t cy, uint16_t fgColor, uint16_t bgColor) {
  switch (iconId) {
    case 0: // HANDYCAM CAMERA LOGO
      cv->fillRoundRect(cx - 9, cy - 6, 18, 13, 3, fgColor);
      cv->fillRect(cx - 4, cy - 9, 8, 3, fgColor);
      cv->fillCircle(cx, cy, 4, bgColor);
      cv->fillCircle(cx, cy, 2, fgColor);
      cv->fillCircle(cx + 5, cy - 3, 1, bgColor);
      break;

    case 1: // SD GALLERY PHOTO LOGO
      cv->drawRoundRect(cx - 9, cy - 7, 18, 14, 2, fgColor);
      cv->drawRoundRect(cx - 8, cy - 6, 16, 12, 2, fgColor);
      cv->fillCircle(cx - 4, cy - 2, 2, fgColor); // Sun
      // Mountain peaks
      cv->fillTriangle(cx - 7, cy + 5, cx - 1, cy - 1, cx + 4, cy + 5, fgColor);
      cv->fillTriangle(cx - 1, cy + 5, cx + 4, cy - 2, cx + 7, cy + 5, fgColor);
      break;

    case 2: // WIFI SIGNAL LOGO
      cv->fillCircle(cx, cy + 5, 2, fgColor);
      cv->drawLine(cx - 4, cy + 1, cx, cy - 2, fgColor);
      cv->drawLine(cx, cy - 2, cx + 4, cy + 1, fgColor);
      cv->drawLine(cx - 7, cy - 2, cx, cy - 6, fgColor);
      cv->drawLine(cx, cy - 6, cx + 7, cy - 2, fgColor);
      cv->drawLine(cx - 7, cy - 1, cx, cy - 5, fgColor);
      cv->drawLine(cx, cy - 5, cx + 7, cy - 1, fgColor);
      break;

    case 3: // BLUETOOTH RUNE LOGO
      cv->drawLine(cx, cy - 8, cx, cy + 8, fgColor);
      cv->drawLine(cx + 1, cy - 8, cx + 1, cy + 8, fgColor);
      cv->drawLine(cx, cy - 8, cx + 6, cy - 4, fgColor);
      cv->drawLine(cx + 6, cy - 4, cx - 5, cy + 4, fgColor);
      cv->drawLine(cx, cy + 8, cx + 6, cy + 4, fgColor);
      cv->drawLine(cx + 6, cy + 4, cx - 5, cy - 4, fgColor);
      break;

    case 4: // GAMES CONTROLLER LOGO
      cv->fillRoundRect(cx - 9, cy - 5, 18, 10, 4, fgColor);
      // D-pad
      cv->fillRect(cx - 6, cy - 1, 5, 2, bgColor);
      cv->fillRect(cx - 4, cy - 3, 2, 5, bgColor);
      // Action buttons
      cv->fillCircle(cx + 4, cy - 1, 1, bgColor);
      cv->fillCircle(cx + 6, cy + 1, 1, bgColor);
      break;

    default: // WATCH FACE CLOCK LOGO
      cv->drawCircle(cx, cy, 8, fgColor);
      cv->drawCircle(cx, cy, 7, fgColor);
      cv->drawLine(cx, cy, cx, cy - 5, fgColor);
      cv->drawLine(cx, cy, cx + 4, cy + 2, fgColor);
      cv->fillCircle(cx, cy, 1, fgColor);
      break;
  }
}

/**
 * ============================================================================
 * ANDROID WEAR-OS ANIMATED WATCH FACES (60+ FPS Sub-Second Interpolation)
 * ============================================================================
 */
inline void renderHomeWatchFace(Arduino_Canvas *cv, uint8_t faceStyle, bool wifiOn, bool bleOn) {
  cv->fillScreen(COLOR_BG_OBSIDIAN);

  uint32_t nowMs = millis();
  float smoothSec = (nowMs % 60000) / 1000.0f; // 0.000 .. 59.999
  uint8_t ss = (uint8_t)smoothSec;
  uint8_t mm = (uint8_t)((nowMs / 60000) % 60);
  uint8_t hh = (uint8_t)((10 + (nowMs / 3600000)) % 24);

  if (faceStyle == 0) {
    // ------------------------------------------------------------------------
    // FACE 0: WEAR-OS "ULTRA CHRONO" ANALOG + DIGITAL + LIVE ECG HYBRID
    // ------------------------------------------------------------------------
    drawCircularBezelTicks(cv, COLOR_CYAN_ACCENT);

    // Outer sweeping sub-second progress trail dot
    float secDeg = smoothSec * 6.0f;
    int16_t sxDot, syDot;
    polarToScreen(106.0f, secDeg, sxDot, syDot);
    cv->fillCircle(sxDot, syDot, 3, COLOR_CYAN_ACCENT);

    // 12 Hour indices around r=98
    for (int i = 0; i < 12; i++) {
      int16_t ix, iy;
      polarToScreen(98.0f, i * 30.0f, ix, iy);
      cv->fillCircle(ix, iy, (i % 3 == 0) ? 2 : 1, (i % 3 == 0) ? COLOR_TEXT_WHITE : COLOR_TEXT_MUTED);
    }

    // Top Complication Pill (WiFi, BLE, SD count)
    cv->fillRoundRect(68, 32, 104, 16, 8, COLOR_PANEL_SLATE);
    cv->drawRoundRect(68, 32, 104, 16, 8, COLOR_CYAN_ACCENT);
    cv->setTextSize(1);
    cv->setTextColor(COLOR_CYAN_ACCENT);
    cv->setCursor(78, 36);
    cv->printf("%s  %s  %uJPG", wifiOn ? "W+" : "W-", bleOn ? "B+" : "B-", g_gallery.fileCount);

    // Digital Time Window at Center-Top
    cv->setTextColor(COLOR_TEXT_WHITE);
    cv->setTextSize(3);
    cv->setCursor(52, 64);
    cv->printf("%02u:%02u", hh, mm);
    cv->setTextSize(2);
    cv->setTextColor(COLOR_CYAN_ACCENT);
    cv->setCursor(148, 70);
    cv->printf(":%02u", ss);

    // Animated Live Heart-Rate / Telemetry Sine Waveform at Lower Subdial
    cv->fillRoundRect(56, 146, 128, 30, 6, COLOR_PANEL_SLATE);
    cv->drawRoundRect(56, 146, 128, 30, 6, 0x1926);
    int16_t prevY = 161;
    for (int x = 60; x < 180; x += 2) {
      float phase = (x * 0.14f) - (nowMs * 0.008f);
      int16_t wy = (int16_t)(161 + sinf(phase) * 8.0f * cosf(phase * 0.35f));
      cv->drawLine(x - 2, prevY, x, wy, COLOR_EMERALD);
      prevY = wy;
    }

    // Analog Hour, Minute, and Smooth Sweeping Second Hands
    float minDeg  = (mm + smoothSec / 60.0f) * 6.0f;
    float hourDeg = ((hh % 12) + mm / 60.0f) * 30.0f;

    int16_t hx, hy, mx, my, sx, sy, scx, scy;
    polarToScreen(52.0f, hourDeg, hx, hy);
    polarToScreen(76.0f, minDeg, mx, my);
    polarToScreen(92.0f, secDeg, sx, sy);
    polarToScreen(18.0f, secDeg + 180.0f, scx, scy);

    // Thick Hour & Minute Hands
    cv->drawLine(TFT_CENTER_X, TFT_CENTER_Y, hx, hy, COLOR_TEXT_WHITE);
    cv->drawLine(TFT_CENTER_X + 1, TFT_CENTER_Y, hx + 1, hy, COLOR_TEXT_WHITE);
    cv->drawLine(TFT_CENTER_X, TFT_CENTER_Y, mx, my, COLOR_CYAN_ACCENT);
    // Sweeping Second Hand with Counterweight
    cv->drawLine(scx, scy, sx, sy, COLOR_ROSE);
    cv->fillCircle(TFT_CENTER_X, TFT_CENTER_Y, 5, COLOR_TEXT_WHITE);
    cv->fillCircle(TFT_CENTER_X, TFT_CENTER_Y, 2, COLOR_ROSE);
  } else if (faceStyle == 1) {
    // ------------------------------------------------------------------------
    // FACE 1: WEAR-OS "CYBER ORBITAL" COUNTER-ROTATING SATELLITE RINGS
    // ------------------------------------------------------------------------
    drawCircularBezelTicks(cv, COLOR_VIOLET);

    float angle1 = (nowMs * 0.045f);
    float angle2 = -(nowMs * 0.032f);
    float angle3 = (smoothSec * 6.0f);

    cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 96, 0x2145);
    cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 80, 0x2145);
    cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 64, 0x2145);

    // Draw glowing animated arc segments on each orbit ring
    for (int a = 0; a < 90; a += 4) {
      int16_t ox, oy;
      polarToScreen(96.0f, angle1 + a, ox, oy);
      cv->fillCircle(ox, oy, 2, COLOR_CYAN_ACCENT);
      polarToScreen(80.0f, angle2 + a, ox, oy);
      cv->fillCircle(ox, oy, 2, COLOR_VIOLET);
    }

    int16_t satX, satY;
    polarToScreen(64.0f, angle3, satX, satY);
    cv->fillCircle(satX, satY, 4, COLOR_EMERALD);

    // Center Digital Lockup
    cv->setTextColor(COLOR_CYAN_ACCENT);
    cv->setTextSize(1);
    cv->setCursor(86, 82);
    cv->print("CHRONO-S3");

    cv->setTextColor(COLOR_TEXT_WHITE);
    cv->setTextSize(3);
    cv->setCursor(58, 102);
    cv->printf("%02u:%02u", hh, mm);

    cv->setTextColor(COLOR_EMERALD);
    cv->setTextSize(1);
    cv->setCursor(88, 136);
    cv->printf("SEC .%02u", ss);
  } else {
    // ------------------------------------------------------------------------
    // FACE 2: WEAR-OS "RADAR TELEMETRY" SWEEPING HORIZON FACE
    // ------------------------------------------------------------------------
    drawCircularBezelTicks(cv, COLOR_EMERALD);
    cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 84, 0x1264);
    cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 48, 0x1264);
    cv->drawLine(20, TFT_CENTER_Y, 220, TFT_CENTER_Y, 0x1264);
    cv->drawLine(TFT_CENTER_X, 20, TFT_CENTER_X, 220, 0x1264);

    // Sweeping Radar Beam
    float sweepDeg = (nowMs % 4000) * (360.0f / 4000.0f);
    int16_t rx, ry;
    polarToScreen(106.0f, sweepDeg, rx, ry);
    cv->drawLine(TFT_CENTER_X, TFT_CENTER_Y, rx, ry, COLOR_EMERALD);

    cv->fillRoundRect(44, 76, 152, 42, 8, COLOR_PANEL_SLATE);
    cv->drawRoundRect(44, 76, 152, 42, 8, COLOR_EMERALD);
    cv->setTextColor(COLOR_EMERALD);
    cv->setTextSize(2);
    cv->setCursor(54, 88);
    cv->printf("%02u:%02u:%02u", hh, mm, ss);

    cv->setTextSize(1);
    cv->setTextColor(COLOR_TEXT_WHITE);
    cv->setCursor(58, 134);
    cv->printf("CAM: %s", g_camState.initialized ? "M2145 OK" : "VIEWFINDER");
    cv->setCursor(58, 148);
    cv->printf("SD : %u PHOTOS", g_gallery.fileCount);
  }

  cv->setTextSize(1);
  cv->setTextColor(COLOR_TEXT_MUTED);
  cv->setCursor(52, 204);
  cv->print("1xCLICK:MENU  ROT:FACE");
}

/**
 * ============================================================================
 * 6-NODE RADIAL MENU WITH VECTOR LOGOS, DYNAMIC COLORS & SMOOTH HALO RING
 * ============================================================================
 */
inline void renderRadialMenu(Arduino_Canvas *cv, uint8_t activeIdx) {
  static float s_smoothMenuAngle = 0.0f;
  float targetAngle = activeIdx * 60.0f;

  // Shortest-path angular interpolation for silky-smooth cursor ring motion
  float diff = targetAngle - s_smoothMenuAngle;
  while (diff > 180.0f) diff -= 360.0f;
  while (diff < -180.0f) diff += 360.0f;
  s_smoothMenuAngle = fmodf(s_smoothMenuAngle + diff * 0.35f + 360.0f, 360.0f);

  uint16_t themeColor = kMenuColors[activeIdx % 6];

  cv->fillScreen(COLOR_BG_OBSIDIAN);
  drawCircularBezelTicks(cv, themeColor);

  // Subtle connecting orbit ring at r=78
  cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 78, COLOR_PANEL_SLATE);

  // Smoothly animated double selection halo ring
  int16_t hx, hy;
  polarToScreen(78.0f, s_smoothMenuAngle, hx, hy);
  cv->drawCircle(hx, hy, 22, themeColor);
  cv->drawCircle(hx, hy, 23, themeColor);

  // Draw all 6 Menu Nodes + Custom Vector Logos
  for (uint8_t i = 0; i < 6; i++) {
    int16_t nx, ny;
    polarToScreen(78.0f, i * 60.0f, nx, ny);
    bool selected = (i == activeIdx);
    uint16_t nodeBg = selected ? kMenuColors[i] : COLOR_PANEL_SLATE;
    uint16_t iconFg = selected ? COLOR_BG_OBSIDIAN : COLOR_TEXT_WHITE;

    cv->fillCircle(nx, ny, selected ? 19 : 15, nodeBg);
    cv->drawCircle(nx, ny, selected ? 19 : 15, selected ? COLOR_TEXT_WHITE : kMenuColors[i]);
    drawMenuLogo(cv, i, nx, ny, iconFg, nodeBg);
  }

  // Center Hub Card showing Active Menu Item Title & Description
  cv->fillCircle(TFT_CENTER_X, TFT_CENTER_Y, 46, COLOR_PANEL_SLATE);
  cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 46, themeColor);
  cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 45, themeColor);

  cv->setTextSize(1);
  cv->setTextColor(themeColor);
  cv->setCursor(104, 92);
  cv->printf("%u / 6", activeIdx + 1);

  int16_t labelLen = strlen(kMenuLabels[activeIdx]);
  cv->setTextColor(COLOR_TEXT_WHITE);
  cv->setCursor(TFT_CENTER_X - (labelLen * 3), 108);
  cv->print(kMenuLabels[activeIdx]);

  int16_t subLen = strlen(kMenuSubtitles[activeIdx]);
  cv->setTextColor(COLOR_TEXT_MUTED);
  cv->setCursor(TFT_CENTER_X - (subLen * 3), 124);
  cv->print(kMenuSubtitles[activeIdx]);
}

inline void renderHandyCamScreen(Arduino_Canvas *cv) {
  cv->fillScreen(COLOR_BG_OBSIDIAN);

  if (g_camState.initialized && g_camState.streaming) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb && fb->format == PIXFORMAT_RGB565) {
      cv->draw16bitBeRGBBitmap(0, 0, (uint16_t *)fb->buf, fb->width, fb->height);
    }
    if (fb) esp_camera_fb_return(fb);
  } else {
    cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 76, COLOR_PANEL_SLATE);
    cv->setTextSize(1);
    cv->setTextColor(COLOR_AMBER);
    cv->setCursor(54, 114);
    cv->print(g_camState.initialized ? "STREAM PAUSED" : "CAM NOT INITIALIZED");
  }

  drawCircularBezelTicks(cv, COLOR_CYAN_ACCENT);
  cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 42, COLOR_CYAN_ACCENT);

  static const char *kFilterNames[4] = {"NORM", "IR", "THERM", "CYBER"};
  cv->setTextColor(COLOR_ROSE);
  cv->setTextSize(1);
  cv->setCursor(52, 22);
  cv->printf("%s %ux %s EV%+d",
    g_camState.streaming ? "REC" : "PAU",
    g_camState.zoomLevel,
    kFilterNames[g_camState.filterMode % 4],
    (int)g_camState.exposureEv
  );

  static const char *kToolNames[5] = {"SNAP SD", "STREAM", "ZOOM", "FILTER", "EXPOSURE"};
  cv->fillRoundRect(64, 192, 112, 20, 8, COLOR_CYAN_ACCENT);
  cv->setTextColor(COLOR_BG_OBSIDIAN);
  cv->setCursor(76, 198);
  cv->printf("[%s]", kToolNames[g_camState.activeTool % 5]);
}

inline void renderSDGalleryScreen(Arduino_Canvas *cv) {
  cv->fillScreen(COLOR_BG_OBSIDIAN);

  if (g_gallery.fileCount == 0) {
    drawCircularBezelTicks(cv, COLOR_EMERALD);
    cv->setTextColor(COLOR_EMERALD);
    cv->setTextSize(1);
    cv->setCursor(62, 24);
    cv->print("/sdcard/DCIM (0)");
    drawMenuLogo(cv, 1, TFT_CENTER_X, 90, COLOR_EMERALD, COLOR_BG_OBSIDIAN);
    cv->setTextColor(COLOR_TEXT_MUTED);
    cv->setCursor(62, 118);
    cv->print("NO CAPTURED JPGS");
    return;
  }

  bool previewReady = loadSDPhotoPreview(g_gallery.selectedIndex);

  if (g_gallery.fullscreenMode) {
    if (previewReady && g_gallery.previewBuf != NULL) {
      // Upscale 120x120 preview 2x to fill the 240x240 round GC9A01 display
      uint16_t srcW = g_gallery.decodedW ? g_gallery.decodedW : PREVIEW_W;
      uint16_t srcH = g_gallery.decodedH ? g_gallery.decodedH : PREVIEW_H;
      for (int16_t y = 0; y < 120; y++) {
        uint16_t sy = (uint16_t)((y * srcH) / 120);
        const uint16_t *row = g_gallery.previewBuf + (sy * srcW);
        int16_t dy = y * 2;
        for (int16_t x = 0; x < 120; x++) {
          uint16_t sx = (uint16_t)((x * srcW) / 120);
          uint16_t col = row[sx];
          int16_t dx = x * 2;
          cv->drawPixel(dx,     dy,     col);
          cv->drawPixel(dx + 1, dy,     col);
          cv->drawPixel(dx,     dy + 1, col);
          cv->drawPixel(dx + 1, dy + 1, col);
        }
      }
    } else {
      cv->fillRoundRect(44, 52, 152, 116, 8, COLOR_PANEL_SLATE);
      cv->drawRoundRect(44, 52, 152, 116, 8, COLOR_EMERALD);
      drawMenuLogo(cv, 1, TFT_CENTER_X, 110, COLOR_EMERALD, COLOR_PANEL_SLATE);
    }

    drawCircularBezelTicks(cv, COLOR_EMERALD);

    // Bottom EXIF HUD Pill
    cv->fillRoundRect(42, 180, 156, 34, 8, COLOR_BG_OBSIDIAN);
    cv->drawRoundRect(42, 180, 156, 34, 8, COLOR_EMERALD);
    cv->setTextSize(1);
    cv->setTextColor(COLOR_EMERALD);
    cv->setCursor(54, 186);
    cv->printf("%s (%u/%u)",
      g_gallery.files[g_gallery.selectedIndex].filename,
      g_gallery.selectedIndex + 1,
      g_gallery.fileCount
    );
    cv->setTextColor(COLOR_TEXT_WHITE);
    cv->setCursor(54, 199);
    cv->print("ROT:NEXT  1x/2x:GRID");
    return;
  }

  drawCircularBezelTicks(cv, COLOR_EMERALD);
  cv->setTextColor(COLOR_EMERALD);
  cv->setTextSize(1);
  cv->setCursor(62, 24);
  cv->printf("/sdcard/DCIM (%u)", g_gallery.fileCount);

  uint16_t pageStart = (g_gallery.selectedIndex / 4) * 4;
  for (uint8_t i = 0; i < 4; i++) {
    uint16_t fileIdx = pageStart + i;
    if (fileIdx >= g_gallery.fileCount) break;

    int16_t gx = (i % 2 == 0) ? 52 : 124;
    int16_t gy = (i < 2) ? 52 : 114;
    bool sel = (fileIdx == g_gallery.selectedIndex);

    cv->fillRoundRect(gx, gy, 64, 54, 6, COLOR_PANEL_SLATE);

    // Render live miniature color preview inside the selected thumbnail card
    if (sel && previewReady && g_gallery.previewBuf != NULL) {
      uint16_t srcW = g_gallery.decodedW ? g_gallery.decodedW : PREVIEW_W;
      uint16_t srcH = g_gallery.decodedH ? g_gallery.decodedH : PREVIEW_H;
      for (int16_t ty = 0; ty < 34; ty++) {
        uint16_t sy = (uint16_t)((ty * srcH) / 34);
        const uint16_t *row = g_gallery.previewBuf + (sy * srcW);
        for (int16_t tx = 0; tx < 58; tx++) {
          uint16_t sx = (uint16_t)((tx * srcW) / 58);
          cv->drawPixel(gx + 3 + tx, gy + 3 + ty, row[sx]);
        }
      }
    } else {
      drawMenuLogo(cv, 1, gx + 32, gy + 19, sel ? COLOR_EMERALD : COLOR_TEXT_MUTED, COLOR_PANEL_SLATE);
    }

    cv->drawRoundRect(gx, gy, 64, 54, 6, sel ? COLOR_EMERALD : COLOR_TEXT_MUTED);
    if (sel) {
      cv->drawRoundRect(gx - 1, gy - 1, 66, 56, 7, COLOR_TEXT_WHITE);
    }

    cv->setTextColor(sel ? COLOR_EMERALD : COLOR_TEXT_WHITE);
    cv->setCursor(gx + 5, gy + 41);
    char shortName[10];
    strncpy(shortName, g_gallery.files[fileIdx].filename, 8);
    shortName[8] = '\0';
    cv->print(shortName);
  }

  cv->setTextColor(COLOR_TEXT_MUTED);
  cv->setCursor(46, 196);
  cv->print("1x:VIEW ROT:SEL HOLD:HOME");
}

inline void renderWifiScreen(Arduino_Canvas *cv, bool wifiEnabled) {
  cv->fillScreen(COLOR_BG_OBSIDIAN);
  drawCircularBezelTicks(cv, wifiEnabled ? COLOR_EMERALD : COLOR_CYAN_ACCENT);

  drawMenuLogo(cv, 2, TFT_CENTER_X, 34, wifiEnabled ? COLOR_EMERALD : COLOR_TEXT_MUTED, COLOR_BG_OBSIDIAN);
  cv->setTextColor(COLOR_CYAN_ACCENT);
  cv->setTextSize(1);
  cv->setCursor(62, 50);
  cv->print("ESP32-S3 2.4GHz WIFI");

  cv->fillRoundRect(42, 68, 156, 36, 8, COLOR_PANEL_SLATE);
  cv->drawRoundRect(42, 68, 156, 36, 8, wifiEnabled ? COLOR_EMERALD : COLOR_TEXT_MUTED);
  cv->setTextColor(wifiEnabled ? COLOR_EMERALD : COLOR_AMBER);
  cv->setCursor(52, 76);
  cv->printf("RADIO : %s", wifiEnabled ? "ON (8.5dBm ECO)" : "OFF (COOL MODE)");
  cv->setTextColor(COLOR_TEXT_WHITE);
  cv->setCursor(52, 90);
  cv->printf("POWER : %s", wifiEnabled ? "~75mA LOW-TX" : "0mA (SAVES HEAT)");

  cv->setTextColor(COLOR_TEXT_MUTED);
  cv->setCursor(50, 118);
  cv->print("SSID: ChronoS3_HandyCam");
  cv->setCursor(50, 134);
  if (wifiEnabled) {
    cv->printf("IP  : %s", WiFi.softAPIP().toString().c_str());
  } else {
    cv->print("IP  : 0.0.0.0 (RADIO OFF)");
  }

  // Interactive ON / OFF Action Button Pill
  cv->fillRoundRect(56, 160, 128, 22, 8, wifiEnabled ? COLOR_ROSE : COLOR_EMERALD);
  cv->setTextColor(COLOR_BG_OBSIDIAN);
  cv->setCursor(66, 167);
  cv->print(wifiEnabled ? "[1x: TURN WIFI OFF]" : "[1x: TURN WIFI ON ]");

  cv->setTextColor(COLOR_TEXT_MUTED);
  cv->setCursor(52, 198);
  cv->print("1x:ON/OFF  HOLD:HOME");
}

inline void renderBleScreen(Arduino_Canvas *cv, bool bleEnabled) {
  cv->fillScreen(COLOR_BG_OBSIDIAN);
  drawCircularBezelTicks(cv, bleEnabled ? COLOR_VIOLET : COLOR_TEXT_MUTED);

  drawMenuLogo(cv, 3, TFT_CENTER_X, 34, bleEnabled ? COLOR_VIOLET : COLOR_TEXT_MUTED, COLOR_BG_OBSIDIAN);
  cv->setTextColor(COLOR_VIOLET);
  cv->setTextSize(1);
  cv->setCursor(62, 50);
  cv->print("BLE 5.0 GATT & HID");

  cv->fillRoundRect(42, 68, 156, 36, 8, COLOR_PANEL_SLATE);
  cv->drawRoundRect(42, 68, 156, 36, 8, bleEnabled ? COLOR_VIOLET : COLOR_TEXT_MUTED);
  cv->setTextColor(bleEnabled ? COLOR_VIOLET : COLOR_AMBER);
  cv->setCursor(52, 76);
  cv->printf("RADIO : %s", bleEnabled ? "ON (ADVERTISING)" : "OFF (COOL MODE)");
  cv->setTextColor(COLOR_TEXT_WHITE);
  cv->setCursor(52, 90);
  cv->printf("POWER : %s", bleEnabled ? "~35mA ACTIVE" : "0mA (SAVES HEAT)");

  cv->setTextColor(COLOR_TEXT_MUTED);
  cv->setCursor(52, 122);
  cv->print("DEV : ChronoS3_Watch");
  cv->setCursor(52, 138);
  cv->printf("CPU : %u MHz COOL ECO", (unsigned)getCpuFrequencyMhz());

  // Interactive ON / OFF Action Button Pill
  cv->fillRoundRect(56, 160, 128, 22, 8, bleEnabled ? COLOR_ROSE : COLOR_VIOLET);
  cv->setTextColor(bleEnabled ? COLOR_BG_OBSIDIAN : COLOR_TEXT_WHITE);
  cv->setCursor(68, 167);
  cv->print(bleEnabled ? "[1x: TURN BLE OFF]" : "[1x: TURN BLE ON ]");

  cv->setTextColor(COLOR_TEXT_MUTED);
  cv->setCursor(52, 198);
  cv->print("1x:ON/OFF  HOLD:HOME");
}

/**
 * ============================================================================
 * 3-GAME SMOOTH ROTARY ARCADE SUITE (60+ FPS Delta-Time + Smooth Interpolation)
 * ============================================================================
 * Game 0: ORBIT BREAKER  (360° Smooth Perimeter Paddle + 2-Ring Core Bricks)
 * Game 1: RADAR DEFENSE  (360° Smooth Turret + Twin Laser Burst + EMP Wave)
 * Game 2: CYBER TUNNEL   (360° Polar Vortex Glider + Expanding Portal Rings)
 */
static uint8_t  g_gameMode          = 0;     // 0=Lobby, 1=Playing, 2=GameOver
static uint8_t  g_selectedGame      = 0;     // 0, 1, 2
static float    g_smoothPlayerDeg   = 90.0f; // Smoothly interpolated rotary angle
static uint16_t g_gameScore         = 0;
static uint16_t g_gameBest          = 140;
static uint8_t  g_gameLives         = 3;
static uint32_t g_lastGameStepMs    = 0;
static uint32_t g_boostUntilMs      = 0;

// Game 0 (Orbit Breaker) state + 4-point ball trail
static float    g_ballX             = 120.0f;
static float    g_ballY             = 76.0f;
static float    g_ballVx            = 84.0f;
static float    g_ballVy            = 98.0f;
static float    g_trailX[4]         = {120, 120, 120, 120};
static float    g_trailY[4]         = {76, 76, 76, 76};
static uint16_t g_brickMask         = 0xFFFF; // 16 bricks across 2 concentric rings

// Game 1 (Radar Defense) state
static float    g_boltDist[6]       = {-1, -1, -1, -1, -1, -1};
static float    g_boltDeg[6]        = {0, 0, 0, 0, 0, 0};
static float    g_droneDist[4]      = {104.0f, 112.0f, 96.0f, 108.0f};
static float    g_droneDeg[4]       = {35.0f, 125.0f, 215.0f, 305.0f};

// Game 2 (Cyber Tunnel) state
static float    g_gateRadius[3]     = {16.0f, 48.0f, 80.0f};
static float    g_gateGapDeg[3]     = {60.0f, 185.0f, 305.0f};
static bool     g_gateScored[3]     = {false, false, false};

inline void fireRadarBolt(float aimDeg) {
  for (int i = 0; i < 6; i++) {
    if (g_boltDist[i] < 0.0f) {
      g_boltDist[i] = 16.0f;
      g_boltDeg[i]  = fmodf(aimDeg + 360.0f, 360.0f);
      break;
    }
  }
}

inline void startArcadeGame(uint8_t gameIdx, int16_t currentPaddleDeg) {
  g_selectedGame    = gameIdx % 3;
  g_gameMode        = 1;
  g_gameScore       = 0;
  g_gameLives       = 3;
  g_smoothPlayerDeg = (float)currentPaddleDeg;
  g_lastGameStepMs  = millis();
  g_boostUntilMs    = 0;

  g_ballX     = 120.0f;
  g_ballY     = 76.0f;
  g_ballVx    = 84.0f;
  g_ballVy    = 98.0f;
  g_brickMask = 0xFFFF;
  for (int i = 0; i < 4; i++) {
    g_trailX[i] = g_ballX;
    g_trailY[i] = g_ballY;
  }

  for (int i = 0; i < 6; i++) g_boltDist[i] = -1.0f;
  for (int i = 0; i < 4; i++) {
    g_droneDist[i] = 100.0f + i * 6.0f;
    g_droneDeg[i]  = (float)(i * 90 + 35);
  }

  for (int i = 0; i < 3; i++) {
    g_gateRadius[i] = 16.0f + i * 32.0f;
    g_gateGapDeg[i] = (float)((i * 120 + 60) % 360);
    g_gateScored[i] = false;
  }
}

inline bool handleGamesRotaryEvent(RotaryEvent evt, int16_t &paddleDeg) {
  if (g_gameMode == 0) {
    if (evt == EVT_CW) g_selectedGame = (g_selectedGame + 1) % 3;
    else if (evt == EVT_CCW) g_selectedGame = (g_selectedGame + 2) % 3;
    else if (evt == EVT_SINGLE_CLICK) startArcadeGame(g_selectedGame, paddleDeg);
    else if (evt == EVT_DOUBLE_CLICK) return true;
    return false;
  }

  if (g_gameMode == 1) {
    if (evt == EVT_CW) {
      paddleDeg = (paddleDeg + 16) % 360;
      if (g_selectedGame == 1) fireRadarBolt((float)paddleDeg);
    } else if (evt == EVT_CCW) {
      paddleDeg = (paddleDeg + 344) % 360;
      if (g_selectedGame == 1) fireRadarBolt((float)paddleDeg);
    } else if (evt == EVT_SINGLE_CLICK) {
      g_boostUntilMs = millis() + 650;
      if (g_selectedGame == 0) {
        g_ballVx *= 1.14f;
        g_ballVy *= 1.14f;
      } else if (g_selectedGame == 1) {
        fireRadarBolt((float)paddleDeg - 7.0f);
        fireRadarBolt((float)paddleDeg + 7.0f);
      }
    } else if (evt == EVT_DOUBLE_CLICK) {
      g_gameMode = 0;
    }
    return false;
  }

  if (evt == EVT_SINGLE_CLICK) startArcadeGame(g_selectedGame, paddleDeg);
  else if (evt == EVT_DOUBLE_CLICK) g_gameMode = 0;
  return false;
}

inline void renderGamesScreen(Arduino_Canvas *cv, int16_t paddleDeg) {
  cv->fillScreen(COLOR_BG_OBSIDIAN);

  static const char *kTitles[3] = {
    "01. ORBIT BREAKER",
    "02. RADAR DEFENSE",
    "03. CYBER TUNNEL"
  };
  static const char *kSubs[3] = {
    "360 Paddle Brick Pong",
    "Rotary Turret Shooter",
    "Vortex Gate Dodger"
  };
  static const uint16_t kGameColors[3] = {
    COLOR_CYAN_ACCENT, COLOR_AMBER, COLOR_VIOLET
  };

  // 1. RICH COLOR-CHANGING GAME SELECT LOBBY
  if (g_gameMode == 0) {
    uint16_t lobbyAccent = kGameColors[g_selectedGame % 3];
    drawCircularBezelTicks(cv, lobbyAccent);
    drawMenuLogo(cv, 4, TFT_CENTER_X, 28, lobbyAccent, COLOR_BG_OBSIDIAN);

    cv->setTextSize(1);
    cv->setTextColor(lobbyAccent);
    cv->setCursor(62, 42);
    cv->printf("ARCADE BEST: %04u", g_gameBest);

    for (uint8_t i = 0; i < 3; i++) {
      int16_t by = 58 + i * 40;
      bool sel = (g_selectedGame == i);
      uint16_t cardBorder = sel ? kGameColors[i] : COLOR_TEXT_MUTED;
      uint16_t cardBg     = sel ? 0x1926 : COLOR_PANEL_SLATE;

      cv->fillRoundRect(38, by, 164, 34, 6, cardBg);
      cv->drawRoundRect(38, by, 164, 34, 6, cardBorder);
      if (sel) {
        cv->drawRoundRect(37, by - 1, 166, 36, 7, COLOR_TEXT_WHITE);
      }

      cv->setTextColor(sel ? kGameColors[i] : COLOR_TEXT_WHITE);
      cv->setCursor(46, by + 6);
      cv->print(kTitles[i]);

      cv->setTextColor(sel ? COLOR_TEXT_WHITE : COLOR_TEXT_MUTED);
      cv->setCursor(46, by + 19);
      cv->print(kSubs[i]);
    }

    cv->setTextColor(COLOR_TEXT_WHITE);
    cv->setCursor(48, 198);
    cv->print("1x:PLAY  ROT:GAME  2x:BACK");
    return;
  }

  // 2. GAME OVER MODAL
  if (g_gameMode == 2) {
    drawCircularBezelTicks(cv, COLOR_ROSE);
    cv->fillRoundRect(42, 62, 156, 110, 10, COLOR_PANEL_SLATE);
    cv->drawRoundRect(42, 62, 156, 110, 10, COLOR_ROSE);

    cv->setTextSize(2);
    cv->setTextColor(COLOR_ROSE);
    cv->setCursor(66, 76);
    cv->print("GAME OVER");

    cv->setTextSize(1);
    cv->setTextColor(COLOR_TEXT_WHITE);
    cv->setCursor(76, 106);
    cv->printf("SCORE : %04u", g_gameScore);
    cv->setTextColor(COLOR_AMBER);
    cv->setCursor(76, 122);
    cv->printf("BEST  : %04u", g_gameBest);

    cv->fillRoundRect(54, 140, 132, 20, 6, COLOR_CYAN_ACCENT);
    cv->setTextColor(COLOR_BG_OBSIDIAN);
    cv->setCursor(62, 146);
    cv->print("1x:RETRY  2x:LOBBY");
    return;
  }

  // 3. SMOOTH 60+ FPS DELTA-TIME GAMEPLAY ENGINE
  uint32_t nowMs = millis();
  float dt = (nowMs - g_lastGameStepMs) / 1000.0f;
  if (dt <= 0.0f || dt > 0.08f) dt = 0.016f;
  g_lastGameStepMs = nowMs;

  // Shortest-path angular interpolation so rotary paddle/turret/ship glides smoothly!
  float angDiff = (float)paddleDeg - g_smoothPlayerDeg;
  while (angDiff > 180.0f) angDiff -= 360.0f;
  while (angDiff < -180.0f) angDiff += 360.0f;
  g_smoothPlayerDeg = fmodf(g_smoothPlayerDeg + angDiff * 0.42f + 360.0f, 360.0f);

  bool boostActive = (nowMs < g_boostUntilMs);

  if (g_selectedGame == 0) {
    // ========================================================================
    // GAME 0: ORBIT BREAKER (Smooth 360° Paddle + 2-Ring Core + Ball Trail)
    // ========================================================================
    drawCircularBezelTicks(cv, boostActive ? COLOR_EMERALD : COLOR_CYAN_ACCENT);
    cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 102, 0x10E4);
    cv->fillCircle(TFT_CENTER_X, TFT_CENTER_Y, 10, COLOR_PANEL_SLATE);
    cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 10, COLOR_CYAN_ACCENT);

    if (g_brickMask == 0) g_brickMask = 0xFFFF;
    float ringOffset = (nowMs * 0.02f);

    for (int i = 0; i < 16; i++) {
      if (!(g_brickMask & (1 << i))) continue;
      float bRad = (i < 8) ? 30.0f : 48.0f;
      float bAng = (i % 8) * 45.0f + ((i < 8) ? ringOffset : -ringOffset);
      int16_t bx, by;
      polarToScreen(bRad, bAng, bx, by);
      uint16_t bCol = (i < 8) ? COLOR_AMBER : COLOR_VIOLET;
      cv->fillCircle(bx, by, 5, bCol);
      cv->drawCircle(bx, by, 5, COLOR_TEXT_WHITE);

      float bdx = g_ballX - bx;
      float bdy = g_ballY - by;
      if (bdx * bdx + bdy * bdy <= 85.0f) {
        g_brickMask &= ~(1 << i);
        g_ballVx = -g_ballVx;
        g_ballVy = -g_ballVy;
        g_gameScore += 25;
        if (g_gameScore > g_gameBest) g_gameBest = g_gameScore;
      }
    }

    // Shift ball motion trail
    for (int t = 3; t > 0; t--) {
      g_trailX[t] = g_trailX[t - 1];
      g_trailY[t] = g_trailY[t - 1];
    }
    g_trailX[0] = g_ballX;
    g_trailY[0] = g_ballY;

    g_ballX += g_ballVx * dt;
    g_ballY += g_ballVy * dt;

    float dx = g_ballX - TFT_CENTER_X;
    float dy = g_ballY - TFT_CENTER_Y;
    float dist = sqrtf(dx * dx + dy * dy);
    float halfSpan = boostActive ? 36.0f : 26.0f;

    if (dist >= 96.0f) {
      float ballDeg = fmodf(atan2f(dy, dx) * (180.0f / (float)M_PI) + 90.0f + 360.0f, 360.0f);
      float pDiff = shortestAngleDiffDeg(ballDeg, g_smoothPlayerDeg);

      if (pDiff <= halfSpan + 6.0f) {
        float nx = dx / dist;
        float ny = dy / dist;
        g_ballX = TFT_CENTER_X + nx * 93.0f;
        g_ballY = TFT_CENTER_Y + ny * 93.0f;

        float speed = sqrtf(g_ballVx * g_ballVx + g_ballVy * g_ballVy);
        if (speed > 155.0f) speed = 155.0f;
        float bounceRad = atan2f(-dy, -dx) + ((pDiff / halfSpan) * 0.35f);
        g_ballVx = cosf(bounceRad) * speed;
        g_ballVy = sinf(bounceRad) * speed;
        g_gameScore += 10;
        if (g_gameScore > g_gameBest) g_gameBest = g_gameScore;
      } else if (dist > 112.0f) {
        if (g_gameLives > 1) {
          g_gameLives--;
          g_ballX = 120.0f;
          g_ballY = 76.0f;
        } else {
          g_gameMode = 2;
        }
      }
    }

    // Smooth curved paddle arc around r=102
    uint16_t padCol = boostActive ? COLOR_EMERALD : COLOR_CYAN_ACCENT;
    for (float a = -halfSpan; a <= halfSpan; a += 3.0f) {
      int16_t px, py;
      polarToScreen(102.0f, g_smoothPlayerDeg + a, px, py);
      cv->fillCircle(px, py, 3, padCol);
    }

    // Draw ball motion trail + glowing plasma ball
    for (int t = 3; t >= 1; t--) {
      cv->fillCircle((int16_t)g_trailX[t], (int16_t)g_trailY[t], 4 - t, COLOR_PANEL_SLATE);
    }
    cv->fillCircle((int16_t)g_ballX, (int16_t)g_ballY, 5, COLOR_AMBER);
    cv->fillCircle((int16_t)g_ballX, (int16_t)g_ballY, 2, COLOR_TEXT_WHITE);
  } else if (g_selectedGame == 1) {
    // ========================================================================
    // GAME 1: RADAR DEFENSE (Smooth 360° Turret + Twin Laser + EMP Shockwave)
    // ========================================================================
    drawCircularBezelTicks(cv, COLOR_AMBER);
    cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 44, 0x10E4);
    cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 80, 0x10E4);

    // Dotted laser sight
    for (int r = 22; r < 104; r += 8) {
      int16_t lx, ly;
      polarToScreen((float)r, g_smoothPlayerDeg, lx, ly);
      cv->drawPixel(lx, ly, COLOR_AMBER);
    }

    // Center rotating turret
    int16_t tx, ty;
    polarToScreen(18.0f, g_smoothPlayerDeg, tx, ty);
    cv->drawLine(TFT_CENTER_X, TFT_CENTER_Y, tx, ty, COLOR_CYAN_ACCENT);
    cv->fillCircle(TFT_CENTER_X, TFT_CENTER_Y, 9, COLOR_AMBER);
    cv->fillCircle(TFT_CENTER_X, TFT_CENTER_Y, 4, COLOR_TEXT_WHITE);

    if (boostActive) {
      cv->drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 28, COLOR_CYAN_ACCENT);
    }

    for (int i = 0; i < 6; i++) {
      if (g_boltDist[i] >= 0.0f) {
        g_boltDist[i] += 175.0f * dt;
        int16_t bx, by;
        polarToScreen(g_boltDist[i], g_boltDeg[i], bx, by);
        cv->fillCircle(bx, by, 3, COLOR_CYAN_ACCENT);
        if (g_boltDist[i] > 114.0f) g_boltDist[i] = -1.0f;
      }
    }

    for (int d = 0; d < 4; d++) {
      g_droneDist[d] -= (22.0f + d * 2.0f) * dt;
      int16_t dx, dy;
      polarToScreen(g_droneDist[d], g_droneDeg[d], dx, dy);
      cv->fillCircle(dx, dy, 6, COLOR_ROSE);
      cv->drawCircle(dx, dy, 6, COLOR_TEXT_WHITE);

      for (int b = 0; b < 6; b++) {
        if (g_boltDist[b] < 0.0f) continue;
        float bDiff = shortestAngleDiffDeg(g_boltDeg[b], g_droneDeg[d]);
        if (bDiff <= 16.0f && fabsf(g_boltDist[b] - g_droneDist[d]) <= 12.0f) {
          g_boltDist[b]  = -1.0f;
          g_droneDist[d] = 112.0f;
          g_droneDeg[d]  = fmodf(g_droneDeg[d] + 105.0f + d * 19.0f, 360.0f);
          g_gameScore   += 25;
          if (g_gameScore > g_gameBest) g_gameBest = g_gameScore;
          break;
        }
      }

      if (g_droneDist[d] <= 15.0f) {
        g_droneDist[d] = 112.0f;
        if (g_gameLives > 1) g_gameLives--;
        else g_gameMode = 2;
      }
    }
  } else {
    // ========================================================================
    // GAME 2: CYBER TUNNEL (Smooth 360° Polar Vortex Gate Runner)
    // ========================================================================
    drawCircularBezelTicks(cv, COLOR_VIOLET);
    cv->fillCircle(TFT_CENTER_X, TFT_CENTER_Y, 8, COLOR_VIOLET);

    for (int g = 0; g < 3; g++) {
      g_gateRadius[g] += 40.0f * dt;
      float r = g_gateRadius[g];

      for (int deg = 0; deg < 360; deg += 12) {
        float d = shortestAngleDiffDeg((float)deg, g_gateGapDeg[g]);
        bool safe = (d <= 38.0f);
        int16_t gx, gy;
        polarToScreen(r, (float)deg, gx, gy);
        cv->fillCircle(gx, gy, safe ? 1 : 2, safe ? COLOR_EMERALD : COLOR_ROSE);
      }

      if (!g_gateScored[g] && r >= 90.0f && r <= 98.0f) {
        g_gateScored[g] = true;
        float gapDiff = shortestAngleDiffDeg(g_smoothPlayerDeg, g_gateGapDeg[g]);
        if (gapDiff <= 40.0f || boostActive) {
          g_gameScore += 30;
          if (g_gameScore > g_gameBest) g_gameBest = g_gameScore;
        } else {
          if (g_gameLives > 1) g_gameLives--;
          else g_gameMode = 2;
        }
      }

      if (r > 110.0f) {
        g_gateRadius[g] = 12.0f;
        g_gateGapDeg[g] = fmodf(g_gateGapDeg[g] + 125.0f + g * 35.0f, 360.0f);
        g_gateScored[g] = false;
      }
    }

    int16_t sx, sy;
    polarToScreen(94.0f, g_smoothPlayerDeg, sx, sy);
    cv->fillCircle(sx, sy, boostActive ? 8 : 6, boostActive ? COLOR_EMERALD : COLOR_CYAN_ACCENT);
    cv->drawCircle(sx, sy, boostActive ? 9 : 7, COLOR_TEXT_WHITE);
  }

  // Top Arcade HUD Pill
  cv->fillRoundRect(54, 14, 132, 16, 6, COLOR_BG_OBSIDIAN);
  cv->drawRoundRect(54, 14, 132, 16, 6, COLOR_PANEL_SLATE);
  cv->setTextSize(1);
  cv->setTextColor(COLOR_TEXT_WHITE);
  cv->setCursor(62, 18);
  cv->printf("SCR:%04u  LIVES:%u", g_gameScore, g_gameLives);
}

#endif // UI_GC9A01_ROUND_H
