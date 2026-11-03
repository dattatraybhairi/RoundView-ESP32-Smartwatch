/**
 * ============================================================================
 * FILE: /arduino/esp32s3_gc9a01_smartwatch/camera_handycam.h
 * PROJECT: ChronoS3 — OV2640 / M2145 HandyCam Live Stream Driver (Cool Eco Mode)
 * ============================================================================
 * Thermal & DMA optimizations for ESP32-S3 + M2145:
 *  - Uses CAMERA_GRAB_WHEN_EMPTY with fb_count = 1 so the ESP32-S3 LCD_CAM DMA
 *    automatically pauses whenever not on SCREEN_CAMERA (zero background DMA heat).
 *  - Runs XCLK at 16 MHz (instead of 20 MHz) to reduce M2145 sensor & DMA heat
 *    while maintaining smooth 240x240 viewfinder framerates.
 */

#ifndef CAMERA_HANDYCAM_H
#define CAMERA_HANDYCAM_H

#include <Arduino.h>
#include "esp_camera.h"
#include "pin_config.h"

enum CameraControlTool {
  CAM_TOOL_SNAP = 0,
  CAM_TOOL_STREAM,
  CAM_TOOL_ZOOM,
  CAM_TOOL_FILTER,
  CAM_TOOL_EXPOSURE,
  CAM_TOOL_COUNT
};

struct HandyCamState {
  bool initialized = false;
  bool streaming = true;
  uint8_t activeTool = 0;
  uint8_t zoomLevel = 1;     // 1x, 2x, 4x
  uint8_t filterMode = 0;    // 0=Normal, 1=IR_Night, 2=Thermal, 3=Cyber
  int8_t exposureEv = 0;     // -2 .. +2
  uint16_t fps = 24;
};

static HandyCamState g_camState;

inline bool initOV2640Camera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = CAM_PIN_D0;
  config.pin_d1       = CAM_PIN_D1;
  config.pin_d2       = CAM_PIN_D2;
  config.pin_d3       = CAM_PIN_D3;
  config.pin_d4       = CAM_PIN_D4;
  config.pin_d5       = CAM_PIN_D5;
  config.pin_d6       = CAM_PIN_D6;
  config.pin_d7       = CAM_PIN_D7;
  config.pin_xclk     = CAM_PIN_XCLK;
  config.pin_pclk     = CAM_PIN_PCLK;
  config.pin_vsync    = CAM_PIN_VSYNC;
  config.pin_href     = CAM_PIN_HREF;
  config.pin_sccb_sda = CAM_PIN_SIOD;
  config.pin_sccb_scl = CAM_PIN_SIOC;
  config.pin_pwdn     = CAM_PIN_PWDN;
  config.pin_reset    = CAM_PIN_RESET;
  // IMPORTANT: ESP32-S3 LCD_CAM requires 20MHz XCLK (240MHz PLL / 12 = 20MHz)
  // for M2145 (GC2145) / OV2640 SCCB sensor detection & PLL lock!
  config.xclk_freq_hz = 20000000;

  config.frame_size   = FRAMESIZE_240X240;
  config.pixel_format = PIXFORMAT_RGB565;
  config.grab_mode    = CAMERA_GRAB_LATEST;
  config.fb_location  = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count     = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    // Fallback to single framebuffer if PSRAM heap is tight
    config.fb_count  = 1;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    err = esp_camera_init(&config);
  }

  if (err != ESP_OK) {
    Serial.printf("[CAM] Camera init failed: 0x%x\n", err);
    g_camState.initialized = false;
    return false;
  }

  Serial.println("[CAM] M2145 / OV2640 Camera Initialized OK (240x240 RGB565)");
  g_camState.initialized = true;
  return true;
}

inline void cycleCameraFilter() {
  g_camState.filterMode = (g_camState.filterMode + 1) % 4;
}

inline void cycleCameraExposure() {
  g_camState.exposureEv++;
  if (g_camState.exposureEv > 2) g_camState.exposureEv = -2;
}

inline void cycleCameraZoom() {
  if (g_camState.zoomLevel == 1) g_camState.zoomLevel = 2;
  else if (g_camState.zoomLevel == 2) g_camState.zoomLevel = 4;
  else g_camState.zoomLevel = 1;
}

#endif // CAMERA_HANDYCAM_H
