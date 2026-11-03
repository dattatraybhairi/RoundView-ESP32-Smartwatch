import React from 'react';
import {
  WatchState,
  WatchScreenId,
  CameraScene,
  CameraFilter,
  CAMERA_SCENES,
  CAMERA_FILTERS,
  HardwareLogEntry,
} from '../watch/watchStateMachine';
import {
  Camera,
  HardDrive,
  Trash2,
  Eye,
  Video,
  Download,
  Terminal,
} from 'lucide-react';

interface HardwarePeripheralPanelProps {
  state: WatchState;
  onJumpToScreen: (screen: WatchScreenId) => void;
  onSelectCameraScene: (scene: CameraScene) => void;
  onSelectCameraFilter: (filter: CameraFilter) => void;
  onToggleWebcamLive: () => void;
  webcamActive: boolean;
  webcamError: string | null;
  onViewSdImageOnWatch: (index: number) => void;
  onDeleteSdImage: (id: string) => void;
  logs: HardwareLogEntry[];
}

const ALL_SCREENS: { id: WatchScreenId; label: string }[] = [
  { id: 'HOME', label: 'Home Watch' },
  { id: 'MENU', label: 'Radial Menu' },
  { id: 'CAMERA', label: 'HandyCam' },
  { id: 'GALLERY', label: 'SD Gallery' },
  { id: 'WIFI', label: 'WiFi & RTSP' },
  { id: 'BLUETOOTH', label: 'BLE 5.0' },
  { id: 'GAMES', label: '240×240 Games' },
];

export const HardwarePeripheralPanel: React.FC<HardwarePeripheralPanelProps> = ({
  state,
  onJumpToScreen,
  onSelectCameraScene,
  onSelectCameraFilter,
  onToggleWebcamLive,
  webcamActive,
  webcamError,
  onViewSdImageOnWatch,
  onDeleteSdImage,
  logs,
}) => {
  const usedKB = state.sdImages.reduce((acc, item) => acc + item.sizeKB, 0);

  return (
    <div className="flex flex-col gap-5">
      {/* Quick Watch Screen Matrix & Active Screen Telemetry */}
      <div className="bg-[#111827] border border-slate-800 rounded-xl p-5">
        <div className="flex items-center justify-between mb-3">
          <div>
            <h2 className="text-sm font-semibold text-white">
              GC9A01 Screen State & Quick Jump
            </h2>
            <p className="text-xs text-slate-400">
              Navigate via EC11 Rotary Switch or jump directly to inspect any screen
            </p>
          </div>
          <span className="text-xs font-mono text-cyan-400">
            STATE: {state.screen}
          </span>
        </div>

        <div className="flex flex-wrap gap-1.5">
          {ALL_SCREENS.map((scr) => {
            const active = state.screen === scr.id;
            return (
              <button
                key={scr.id}
                type="button"
                onClick={() => onJumpToScreen(scr.id)}
                className={`px-3 py-1.5 rounded-lg text-xs font-medium transition-colors cursor-pointer whitespace-nowrap shrink-0 ${
                  active
                    ? 'bg-cyan-500 text-slate-950 font-semibold'
                    : 'bg-slate-900 text-slate-300 border border-slate-800 hover:border-slate-700 hover:text-white'
                }`}
              >
                {scr.label}
              </button>
            );
          })}
        </div>
      </div>

      {/* Connected Camera (ESP32-S3 OV2640 / HandyCam Feed Bench) */}
      <div className="bg-[#111827] border border-slate-800 rounded-xl p-5 flex flex-col gap-4">
        <div className="flex items-center justify-between border-b border-slate-800 pb-3">
          <div className="flex items-center gap-2.5">
            <Camera className="w-4 h-4 text-cyan-400 shrink-0" />
            <div>
              <h2 className="text-sm font-semibold text-white">
                ESP32-S3 OV2640 HandyCam Stream Source
              </h2>
              <p className="text-xs text-slate-400">
                DVP 8-Bit Camera Bus · 240×240 RGB565 DMA Viewfinder + JPEG Encoder
              </p>
            </div>
          </div>

          <button
            type="button"
            onClick={onToggleWebcamLive}
            className={`flex items-center gap-1.5 px-3 py-1.5 rounded-lg text-xs font-medium transition-colors cursor-pointer whitespace-nowrap shrink-0 ${
              webcamActive
                ? 'bg-rose-500/20 border border-rose-500/50 text-rose-200'
                : 'bg-slate-900 border border-slate-700 text-slate-200 hover:border-cyan-400/60'
            }`}
          >
            <Video className="w-3.5 h-3.5 text-cyan-400" />
            <span>{webcamActive ? 'Stop Browser Webcam' : 'Connect Real Webcam'}</span>
          </button>
        </div>

        {webcamError && (
          <div className="text-xs font-mono text-amber-300 bg-amber-950/40 border border-amber-800/60 rounded-lg px-3 py-2">
            Webcam note: {webcamError} (Using simulated OV2640 hardware feed)
          </div>
        )}

        <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
          <div>
            <label className="block text-xs font-mono text-slate-400 mb-1.5">
              SENSOR SCENE FEED
            </label>
            <div className="grid grid-cols-2 gap-1.5">
              {CAMERA_SCENES.map((scene) => (
                <button
                  key={scene}
                  type="button"
                  onClick={() => onSelectCameraScene(scene)}
                  className={`px-2.5 py-1.5 rounded text-xs font-mono text-left transition-colors cursor-pointer whitespace-nowrap truncate ${
                    state.cameraScene === scene
                      ? 'bg-cyan-500/20 border border-cyan-400 text-cyan-200 font-semibold'
                      : 'bg-slate-900 border border-slate-800 text-slate-400 hover:text-slate-200'
                  }`}
                >
                  {scene}
                </button>
              ))}
            </div>
          </div>

          <div>
            <label className="block text-xs font-mono text-slate-400 mb-1.5">
              OV2640 ISP PIPELINE FILTER
            </label>
            <div className="grid grid-cols-2 gap-1.5">
              {CAMERA_FILTERS.map((flt) => (
                <button
                  key={flt}
                  type="button"
                  onClick={() => onSelectCameraFilter(flt)}
                  className={`px-2.5 py-1.5 rounded text-xs font-mono text-left transition-colors cursor-pointer whitespace-nowrap truncate ${
                    state.cameraFilter === flt
                      ? 'bg-emerald-500/20 border border-emerald-400 text-emerald-200 font-semibold'
                      : 'bg-slate-900 border border-slate-800 text-slate-400 hover:text-slate-200'
                  }`}
                >
                  {flt}
                </button>
              ))}
            </div>
          </div>
        </div>
      </div>

      {/* MicroSD Card (/sdcard/DCIM) Live File System Browser */}
      <div className="bg-[#111827] border border-slate-800 rounded-xl p-5 flex flex-col gap-3">
        <div className="flex items-center justify-between border-b border-slate-800 pb-3">
          <div className="flex items-center gap-2.5">
            <HardDrive className="w-4 h-4 text-emerald-400 shrink-0" />
            <div>
              <h2 className="text-sm font-semibold text-white">
                SD_MMC Card Filesystem (/sdcard/DCIM)
              </h2>
              <p className="text-xs text-slate-400">
                Captured HandyCam 240×240 JPEGs stored on virtual FAT32 volume
              </p>
            </div>
          </div>
          <div className="text-right font-mono text-xs tabular-nums">
            <span className="text-emerald-400 font-semibold">
              {state.sdImages.length} FILES
            </span>
            <span className="text-slate-500 mx-1.5">·</span>
            <span className="text-slate-300">{usedKB} KB USED</span>
          </div>
        </div>

        <div className="divide-y divide-slate-800/80 max-h-52 overflow-y-auto pr-1">
          {state.sdImages.map((img, idx) => (
            <div
              key={img.id}
              className="py-2 flex items-center justify-between gap-3 text-xs font-mono"
            >
              <div className="min-w-0">
                <div className="text-slate-200 font-medium truncate">
                  {img.path}
                </div>
                <div className="text-[11px] text-slate-400">
                  {img.timestamp} · {img.resolution} · {img.sizeKB} KB · {img.filter}
                </div>
              </div>

              <div className="flex items-center gap-1.5 shrink-0">
                <button
                  type="button"
                  aria-label={`Open ${img.filename} on Watch`}
                  onClick={() => onViewSdImageOnWatch(idx)}
                  className="px-2.5 py-1 rounded bg-slate-900 hover:bg-slate-800 border border-slate-700 text-cyan-300 flex items-center gap-1 cursor-pointer whitespace-nowrap"
                >
                  <Eye className="w-3.5 h-3.5" />
                  <span>Open on Watch</span>
                </button>
                {img.dataUrl && (
                  <a
                    href={img.dataUrl}
                    download={img.filename}
                    aria-label={`Download ${img.filename}`}
                    className="p-1.5 rounded bg-slate-900 hover:bg-slate-800 border border-slate-700 text-slate-300"
                    title="Download captured JPEG"
                  >
                    <Download className="w-3.5 h-3.5" />
                  </a>
                )}
                <button
                  type="button"
                  aria-label={`Delete ${img.filename}`}
                  onClick={() => onDeleteSdImage(img.id)}
                  className="p-1.5 rounded bg-slate-900 hover:bg-rose-950/60 border border-slate-800 hover:border-rose-700 text-slate-400 hover:text-rose-300 cursor-pointer"
                  title="Delete from SD card"
                >
                  <Trash2 className="w-3.5 h-3.5" />
                </button>
              </div>
            </div>
          ))}
        </div>
      </div>

      {/* Hardware Interrupt & State Transition Bus Log */}
      <div className="bg-[#111827] border border-slate-800 rounded-xl p-4">
        <div className="flex items-center justify-between mb-2">
          <div className="flex items-center gap-2 text-xs font-mono text-slate-300">
            <Terminal className="w-3.5 h-3.5 text-cyan-400" />
            <span>ESP32-S3 UART0 SERIAL MONITOR (115200 BAUD)</span>
          </div>
          <span className="text-[11px] font-mono text-slate-400">
            ISR + FSM TRACE
          </span>
        </div>
        <div className="bg-[#07090E] border border-slate-800/90 rounded-lg p-2.5 font-mono text-[11px] space-y-1 max-h-28 overflow-y-auto">
          {logs.map((entry) => (
            <div
              key={entry.id}
              className="flex items-center justify-between gap-2 text-slate-300"
            >
              <span className="truncate">
                <span className="text-slate-500">[{entry.time}]</span>{' '}
                <span className="text-cyan-400 font-semibold">
                  {entry.event}
                </span>{' '}
                <span>{entry.detail}</span>
              </span>
              <span className="text-slate-400 shrink-0">{entry.screen}</span>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};
