export type WatchScreenId =
  | 'HOME'
  | 'MENU'
  | 'CAMERA'
  | 'GALLERY'
  | 'WIFI'
  | 'BLUETOOTH'
  | 'GAMES';

export type RotaryEvent = 'CW' | 'CCW' | 'SINGLE_CLICK' | 'DOUBLE_CLICK';

export type WatchFaceStyle = 'CHRONO' | 'ORBITAL' | 'TELEMETRY';

export type CameraFilter = 'NORMAL' | 'IR_NIGHT' | 'THERMAL' | 'CYBER_EDGE';

export type CameraScene =
  | 'LAB_BENCH'
  | 'OUTDOOR_DRONE'
  | 'NIGHT_PATROL'
  | 'WEBCAM_LIVE';

export type CameraControlId =
  | 'SNAP'
  | 'STREAM'
  | 'ZOOM'
  | 'FILTER'
  | 'EXPOSURE'
  | 'SCENE';

export interface SDCardImage {
  id: string;
  filename: string;
  path: string;
  timestamp: string;
  resolution: '240x240';
  sizeKB: number;
  filter: CameraFilter;
  sceneName: CameraScene;
  seedHue: number;
  dataUrl?: string;
}

export interface WifiNetwork {
  ssid: string;
  rssi: number;
  channel: number;
  secure: boolean;
  connected: boolean;
  ip?: string;
}

export interface BleDevice {
  id: string;
  name: string;
  mac: string;
  rssi: number;
  connected: boolean;
  type: 'PHONE' | 'SHUTTER' | 'AUDIO' | 'SENSOR';
}

export type GameId = 'ORBITAL_BREAKER' | 'RADAR_DEFENSE' | 'CYBER_TUNNEL';

export const ARCADE_GAMES: GameId[] = [
  'ORBITAL_BREAKER',
  'RADAR_DEFENSE',
  'CYBER_TUNNEL',
];

export interface HardwareLogEntry {
  id: string;
  time: string;
  event: RotaryEvent | 'SYSTEM' | 'SD_WRITE' | 'NET';
  detail: string;
  screen: WatchScreenId;
}

export interface WatchState {
  screen: WatchScreenId;
  watchFace: WatchFaceStyle;
  menuIndex: number;
  // Camera / HandyCam state
  cameraControlIndex: number;
  cameraStreaming: boolean;
  cameraZoom: 1 | 1.5 | 2 | 4;
  cameraFilter: CameraFilter;
  cameraExposure: -1 | 0 | 1 | 2;
  cameraScene: CameraScene;
  captureCount: number;
  lastCapturedId: string | null;
  // SD Card Gallery state
  sdImages: SDCardImage[];
  galleryIndex: number;
  galleryMode: 'GRID' | 'VIEWER';
  galleryShowExif: boolean;
  // WiFi state
  wifiEnabled: boolean;
  mjpegServerEnabled: boolean;
  wifiScanning: boolean;
  wifiCursor: number;
  wifiNetworks: WifiNetwork[];
  // Bluetooth state
  bleEnabled: boolean;
  bleCursor: number;
  bleShutterCount: number;
  bleDevices: BleDevice[];
  // Games state
  gameMode: 'SELECT' | 'PLAYING' | 'GAMEOVER';
  selectedGame: GameId;
  paddleAngleDeg: number; // 0..359 around 240x240 perimeter
  gameScore: number;
  gameBestScore: number;
  gamePulseCount: number;
  // System telemetry
  batteryPct: number;
  psramUsedMB: number;
  sdTotalMB: number;
  encoderPosition: number; // Cumulative ticks
}

export const MENU_ITEMS: {
  id: WatchScreenId;
  label: string;
  shortDesc: string;
  accentColor: string;
}[] = [
  {
    id: 'CAMERA',
    label: 'HandyCam',
    shortDesc: 'OV2640 Live Stream & Capture',
    accentColor: '#06B6D4',
  },
  {
    id: 'GALLERY',
    label: 'SD Gallery',
    shortDesc: '/sdcard/DCIM Photo Viewer',
    accentColor: '#10B981',
  },
  {
    id: 'WIFI',
    label: 'WiFi & RTSP',
    shortDesc: '2.4GHz AP & MJPEG Server',
    accentColor: '#3B82F6',
  },
  {
    id: 'BLUETOOTH',
    label: 'BLE 5.0 Link',
    shortDesc: 'GATT & HID Remote Shutter',
    accentColor: '#8B5CF6',
  },
  {
    id: 'GAMES',
    label: 'Orbital Games',
    shortDesc: '240x240 Rotary Arcade',
    accentColor: '#F59E0B',
  },
  {
    id: 'HOME',
    label: 'Watch Face',
    shortDesc: 'Return to Chrono Home',
    accentColor: '#EC4899',
  },
];

export const WATCH_FACES: WatchFaceStyle[] = ['CHRONO', 'ORBITAL', 'TELEMETRY'];

export const CAMERA_CONTROLS: { id: CameraControlId; label: string }[] = [
  { id: 'SNAP', label: 'SNAP SD' },
  { id: 'STREAM', label: 'REC STREAM' },
  { id: 'ZOOM', label: 'ZOOM' },
  { id: 'FILTER', label: 'ISP FILTER' },
  { id: 'EXPOSURE', label: 'EV BIAS' },
  { id: 'SCENE', label: 'CAM SOURCE' },
];

export const CAMERA_FILTERS: CameraFilter[] = [
  'NORMAL',
  'IR_NIGHT',
  'THERMAL',
  'CYBER_EDGE',
];

export const CAMERA_SCENES: CameraScene[] = [
  'LAB_BENCH',
  'OUTDOOR_DRONE',
  'NIGHT_PATROL',
  'WEBCAM_LIVE',
];

export const CAMERA_ZOOMS: (1 | 1.5 | 2 | 4)[] = [1, 1.5, 2, 4];
export const CAMERA_EXPOSURES: (-1 | 0 | 1 | 2)[] = [-1, 0, 1, 2];

export const INITIAL_SD_IMAGES: SDCardImage[] = [
  {
    id: 'img-0004',
    filename: 'IMG_0004.JPG',
    path: '/sdcard/DCIM/IMG_0004.JPG',
    timestamp: '2026-11-02 21:48:12',
    resolution: '240x240',
    sizeKB: 48,
    filter: 'NORMAL',
    sceneName: 'LAB_BENCH',
    seedHue: 192,
  },
  {
    id: 'img-0003',
    filename: 'IMG_0003.JPG',
    path: '/sdcard/DCIM/IMG_0003.JPG',
    timestamp: '2026-11-02 20:15:44',
    resolution: '240x240',
    sizeKB: 52,
    filter: 'IR_NIGHT',
    sceneName: 'NIGHT_PATROL',
    seedHue: 145,
  },
  {
    id: 'img-0002',
    filename: 'IMG_0002.JPG',
    path: '/sdcard/DCIM/IMG_0002.JPG',
    timestamp: '2026-11-02 18:04:09',
    resolution: '240x240',
    sizeKB: 61,
    filter: 'THERMAL',
    sceneName: 'OUTDOOR_DRONE',
    seedHue: 28,
  },
  {
    id: 'img-0001',
    filename: 'IMG_0001.JPG',
    path: '/sdcard/DCIM/IMG_0001.JPG',
    timestamp: '2026-11-02 16:30:00',
    resolution: '240x240',
    sizeKB: 44,
    filter: 'CYBER_EDGE',
    sceneName: 'LAB_BENCH',
    seedHue: 280,
  },
];

export function createInitialWatchState(): WatchState {
  return {
    screen: 'HOME',
    watchFace: 'CHRONO',
    menuIndex: 0,
    cameraControlIndex: 0,
    cameraStreaming: true,
    cameraZoom: 1,
    cameraFilter: 'NORMAL',
    cameraExposure: 0,
    cameraScene: 'LAB_BENCH',
    captureCount: 4,
    lastCapturedId: null,
    sdImages: [...INITIAL_SD_IMAGES],
    galleryIndex: 0,
    galleryMode: 'GRID',
    galleryShowExif: true,
    wifiEnabled: true,
    mjpegServerEnabled: true,
    wifiScanning: false,
    wifiCursor: 0,
    wifiNetworks: [
      {
        ssid: 'ESP32S3_Lab_2.4G',
        rssi: -46,
        channel: 6,
        secure: true,
        connected: true,
        ip: '192.168.4.108',
      },
      {
        ssid: 'HandyCam_Field_AP',
        rssi: -61,
        channel: 1,
        secure: true,
        connected: false,
      },
      {
        ssid: 'MakerBench_Mesh',
        rssi: -72,
        channel: 11,
        secure: true,
        connected: false,
      },
      {
        ssid: 'Open_Telemetry_Net',
        rssi: -80,
        channel: 3,
        secure: false,
        connected: false,
      },
    ],
    bleEnabled: true,
    bleCursor: 0,
    bleShutterCount: 0,
    bleDevices: [
      {
        id: 'ble-1',
        name: 'HandyCam Remote Grip',
        mac: 'A4:CF:12:8E:39:01',
        rssi: -42,
        connected: true,
        type: 'SHUTTER',
      },
      {
        id: 'ble-2',
        name: 'Android Companion S3',
        mac: '7C:9E:BD:44:10:8A',
        rssi: -55,
        connected: true,
        type: 'PHONE',
      },
      {
        id: 'ble-3',
        name: 'Polar H10 BLE Sensor',
        mac: 'E8:31:CD:02:71:19',
        rssi: -68,
        connected: false,
        type: 'SENSOR',
      },
    ],
    gameMode: 'SELECT',
    selectedGame: 'ORBITAL_BREAKER',
    paddleAngleDeg: 90,
    gameScore: 0,
    gameBestScore: 140,
    gamePulseCount: 0,
    batteryPct: 88,
    psramUsedMB: 3.4,
    sdTotalMB: 16384,
    encoderPosition: 0,
  };
}

/**
 * Pure state machine reducer for the ESP32-S3 + GC9A01 Smart Watch driven by EC11 Rotary Switch.
 * - CW / CCW: Rotary encoder quadrature steps
 * - SINGLE_CLICK: Short press on encoder push-switch (Select / Action)
 * - DOUBLE_CLICK: Double press on encoder push-switch (Menu Back / Hierarchy Up)
 */
export function reduceWatchState(
  state: WatchState,
  event: RotaryEvent,
  options?: { capturedDataUrl?: string; timestamp?: string }
): WatchState {
  const delta = event === 'CW' ? 1 : event === 'CCW' ? -1 : 0;
  const nextEncoderPosition = state.encoderPosition + delta;

  // 1. HOME SCREEN
  if (state.screen === 'HOME') {
    if (event === 'CW' || event === 'CCW') {
      const idx = WATCH_FACES.indexOf(state.watchFace);
      const nextIdx =
        (idx + delta + WATCH_FACES.length) % WATCH_FACES.length;
      return {
        ...state,
        encoderPosition: nextEncoderPosition,
        watchFace: WATCH_FACES[nextIdx],
      };
    }
    if (event === 'SINGLE_CLICK') {
      return {
        ...state,
        screen: 'MENU',
      };
    }
    // DOUBLE_CLICK on HOME resets to default CHRONO face
    return {
      ...state,
      watchFace: 'CHRONO',
    };
  }

  // 2. RADIAL MAIN MENU SCREEN
  if (state.screen === 'MENU') {
    if (event === 'CW' || event === 'CCW') {
      const count = MENU_ITEMS.length;
      const nextIndex = (state.menuIndex + delta + count) % count;
      return {
        ...state,
        encoderPosition: nextEncoderPosition,
        menuIndex: nextIndex,
      };
    }
    if (event === 'SINGLE_CLICK') {
      const targetScreen = MENU_ITEMS[state.menuIndex].id;
      return {
        ...state,
        screen: targetScreen,
        // Reset sub-modes cleanly when entering a screen
        galleryMode: targetScreen === 'GALLERY' ? 'GRID' : state.galleryMode,
        gameMode: targetScreen === 'GAMES' ? 'SELECT' : state.gameMode,
      };
    }
    if (event === 'DOUBLE_CLICK') {
      return {
        ...state,
        screen: 'HOME',
      };
    }
  }

  // 3. CAMERA / HANDYCAM STREAM SCREEN
  if (state.screen === 'CAMERA') {
    if (event === 'CW' || event === 'CCW') {
      const count = CAMERA_CONTROLS.length;
      const nextControl = (state.cameraControlIndex + delta + count) % count;
      return {
        ...state,
        encoderPosition: nextEncoderPosition,
        cameraControlIndex: nextControl,
      };
    }
    if (event === 'SINGLE_CLICK') {
      const activeControl = CAMERA_CONTROLS[state.cameraControlIndex].id;
      if (activeControl === 'SNAP') {
        const nextNum = state.captureCount + 1;
        const padded = String(nextNum).padStart(4, '0');
        const filename = `IMG_${padded}.JPG`;
        const newImg: SDCardImage = {
          id: `img-${padded}-${Date.now()}`,
          filename,
          path: `/sdcard/DCIM/${filename}`,
          timestamp: options?.timestamp ?? '2026-11-02 22:30:00',
          resolution: '240x240',
          sizeKB: 46 + ((nextNum * 7) % 24),
          filter: state.cameraFilter,
          sceneName: state.cameraScene,
          seedHue: (nextNum * 53) % 360,
          dataUrl: options?.capturedDataUrl,
        };
        return {
          ...state,
          captureCount: nextNum,
          lastCapturedId: newImg.id,
          sdImages: [newImg, ...state.sdImages],
          galleryIndex: 0,
        };
      }
      if (activeControl === 'STREAM') {
        return {
          ...state,
          cameraStreaming: !state.cameraStreaming,
        };
      }
      if (activeControl === 'ZOOM') {
        const idx = CAMERA_ZOOMS.indexOf(state.cameraZoom);
        const nextZoom = CAMERA_ZOOMS[(idx + 1) % CAMERA_ZOOMS.length];
        return {
          ...state,
          cameraZoom: nextZoom,
        };
      }
      if (activeControl === 'FILTER') {
        const idx = CAMERA_FILTERS.indexOf(state.cameraFilter);
        const nextFilter = CAMERA_FILTERS[(idx + 1) % CAMERA_FILTERS.length];
        return {
          ...state,
          cameraFilter: nextFilter,
        };
      }
      if (activeControl === 'EXPOSURE') {
        const idx = CAMERA_EXPOSURES.indexOf(state.cameraExposure);
        const nextExp = CAMERA_EXPOSURES[(idx + 1) % CAMERA_EXPOSURES.length];
        return {
          ...state,
          cameraExposure: nextExp,
        };
      }
      if (activeControl === 'SCENE') {
        const idx = CAMERA_SCENES.indexOf(state.cameraScene);
        const nextScene = CAMERA_SCENES[(idx + 1) % CAMERA_SCENES.length];
        return {
          ...state,
          cameraScene: nextScene,
        };
      }
    }
    if (event === 'DOUBLE_CLICK') {
      return {
        ...state,
        screen: 'MENU',
      };
    }
  }

  // 4. SD CARD GALLERY SCREEN
  if (state.screen === 'GALLERY') {
    const total = state.sdImages.length;
    if (state.galleryMode === 'GRID') {
      if (event === 'CW' || event === 'CCW') {
        if (total === 0) {
          return { ...state, encoderPosition: nextEncoderPosition };
        }
        const nextIdx = (state.galleryIndex + delta + total) % total;
        return {
          ...state,
          encoderPosition: nextEncoderPosition,
          galleryIndex: nextIdx,
        };
      }
      if (event === 'SINGLE_CLICK') {
        if (total === 0) return state;
        return {
          ...state,
          galleryMode: 'VIEWER',
        };
      }
      if (event === 'DOUBLE_CLICK') {
        return {
          ...state,
          screen: 'MENU',
        };
      }
    } else {
      // VIEWER mode: Rotate scrubs photos, Single Click toggles EXIF HUD, Double Click returns to GRID
      if (event === 'CW' || event === 'CCW') {
        if (total === 0) {
          return { ...state, encoderPosition: nextEncoderPosition };
        }
        const nextIdx = (state.galleryIndex + delta + total) % total;
        return {
          ...state,
          encoderPosition: nextEncoderPosition,
          galleryIndex: nextIdx,
        };
      }
      if (event === 'SINGLE_CLICK') {
        return {
          ...state,
          galleryShowExif: !state.galleryShowExif,
        };
      }
      if (event === 'DOUBLE_CLICK') {
        return {
          ...state,
          galleryMode: 'GRID',
        };
      }
    }
  }

  // 5. WIFI SCREEN
  if (state.screen === 'WIFI') {
    // Total interactive rows: 0 = WiFi Toggle, 1 = MJPEG Server Toggle, 2..N = networks
    const totalRows = 2 + state.wifiNetworks.length;
    if (event === 'CW' || event === 'CCW') {
      const nextCursor = (state.wifiCursor + delta + totalRows) % totalRows;
      return {
        ...state,
        encoderPosition: nextEncoderPosition,
        wifiCursor: nextCursor,
      };
    }
    if (event === 'SINGLE_CLICK') {
      if (state.wifiCursor === 0) {
        const nextEnabled = !state.wifiEnabled;
        return {
          ...state,
          wifiEnabled: nextEnabled,
          mjpegServerEnabled: nextEnabled ? state.mjpegServerEnabled : false,
        };
      }
      if (state.wifiCursor === 1) {
        if (!state.wifiEnabled) return state;
        return {
          ...state,
          mjpegServerEnabled: !state.mjpegServerEnabled,
        };
      }
      const netIdx = state.wifiCursor - 2;
      if (netIdx >= 0 && netIdx < state.wifiNetworks.length && state.wifiEnabled) {
        const updated = state.wifiNetworks.map((net, i) => ({
          ...net,
          connected: i === netIdx ? !net.connected : false,
          ip: i === netIdx && !net.connected ? '192.168.4.108' : undefined,
        }));
        return {
          ...state,
          wifiNetworks: updated,
        };
      }
      return state;
    }
    if (event === 'DOUBLE_CLICK') {
      return {
        ...state,
        screen: 'MENU',
      };
    }
  }

  // 6. BLUETOOTH (BLE 5.0) SCREEN
  if (state.screen === 'BLUETOOTH') {
    // 0 = BLE Toggle, 1 = Fire HID Shutter, 2..N = BLE devices
    const totalRows = 2 + state.bleDevices.length;
    if (event === 'CW' || event === 'CCW') {
      const nextCursor = (state.bleCursor + delta + totalRows) % totalRows;
      return {
        ...state,
        encoderPosition: nextEncoderPosition,
        bleCursor: nextCursor,
      };
    }
    if (event === 'SINGLE_CLICK') {
      if (state.bleCursor === 0) {
        return {
          ...state,
          bleEnabled: !state.bleEnabled,
        };
      }
      if (state.bleCursor === 1) {
        if (!state.bleEnabled) return state;
        return {
          ...state,
          bleShutterCount: state.bleShutterCount + 1,
        };
      }
      const devIdx = state.bleCursor - 2;
      if (devIdx >= 0 && devIdx < state.bleDevices.length && state.bleEnabled) {
        const updated = state.bleDevices.map((d, i) =>
          i === devIdx ? { ...d, connected: !d.connected } : d
        );
        return {
          ...state,
          bleDevices: updated,
        };
      }
      return state;
    }
    if (event === 'DOUBLE_CLICK') {
      return {
        ...state,
        screen: 'MENU',
      };
    }
  }

  // 7. GAMES SCREEN (240x240 Circular Arcade)
  if (state.screen === 'GAMES') {
    if (state.gameMode === 'SELECT') {
      if (event === 'CW' || event === 'CCW') {
        const curIdx = ARCADE_GAMES.indexOf(state.selectedGame);
        const nextIdx =
          (curIdx + delta + ARCADE_GAMES.length) % ARCADE_GAMES.length;
        return {
          ...state,
          encoderPosition: nextEncoderPosition,
          selectedGame: ARCADE_GAMES[nextIdx],
        };
      }
      if (event === 'SINGLE_CLICK') {
        return {
          ...state,
          gameMode: 'PLAYING',
          gameScore: 0,
          paddleAngleDeg: 90,
          gamePulseCount: 0,
        };
      }
      if (event === 'DOUBLE_CLICK') {
        return {
          ...state,
          screen: 'MENU',
        };
      }
    } else if (state.gameMode === 'PLAYING') {
      if (event === 'CW' || event === 'CCW') {
        const stepDeg = 15;
        const nextAngle =
          (state.paddleAngleDeg + delta * stepDeg + 360) % 360;
        return {
          ...state,
          encoderPosition: nextEncoderPosition,
          paddleAngleDeg: nextAngle,
        };
      }
      if (event === 'SINGLE_CLICK') {
        return {
          ...state,
          gamePulseCount: state.gamePulseCount + 1,
        };
      }
      if (event === 'DOUBLE_CLICK') {
        return {
          ...state,
          gameMode: 'SELECT',
        };
      }
    } else {
      // GAMEOVER mode
      if (event === 'SINGLE_CLICK') {
        return {
          ...state,
          gameMode: 'PLAYING',
          gameScore: 0,
        };
      }
      if (event === 'DOUBLE_CLICK') {
        return {
          ...state,
          gameMode: 'SELECT',
        };
      }
      return {
        ...state,
        encoderPosition: nextEncoderPosition,
      };
    }
  }

  return state;
}

/**
 * Circular 240x240 GC9A01 display geometry helpers
 */
export const GC9A01_RESOLUTION = 240;
export const GC9A01_RADIUS = 120;
export const GC9A01_CENTER = 120;

export function polarToCartesian(
  cx: number,
  cy: number,
  radius: number,
  angleDeg: number
): { x: number; y: number } {
  const rad = ((angleDeg - 90) * Math.PI) / 180;
  return {
    x: Number((cx + radius * Math.cos(rad)).toFixed(2)),
    y: Number((cy + radius * Math.sin(rad)).toFixed(2)),
  };
}

export function isPointInsideGC9A01(
  x: number,
  y: number,
  safeMarginPx = 0
): boolean {
  const dx = x - GC9A01_CENTER;
  const dy = y - GC9A01_CENTER;
  const allowedRadius = GC9A01_RADIUS - safeMarginPx;
  return dx * dx + dy * dy <= allowedRadius * allowedRadius;
}

export function angularDistanceDeg(a: number, b: number): number {
  const diff = Math.abs(((a - b) % 360) + 540) % 360 - 180;
  return Math.abs(diff);
}
