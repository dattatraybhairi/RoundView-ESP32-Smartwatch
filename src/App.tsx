import React, { useEffect, useRef, useState, useCallback } from 'react';
import {
  createInitialWatchState,
  reduceWatchState,
  RotaryEvent,
  WatchScreenId,
  CameraScene,
  CameraFilter,
  HardwareLogEntry,
  CAMERA_CONTROLS,
} from './watch/watchStateMachine';
import { GC9A01RoundDisplay } from './components/GC9A01RoundDisplay';
import { RotarySwitchDeck } from './components/RotarySwitchDeck';
import { HardwarePeripheralPanel } from './components/HardwarePeripheralPanel';
import { FirmwareAndPinoutSection } from './components/FirmwareAndPinoutSection';
import { ESP32S3_ARDUINO_FIRMWARE } from './watch/esp32FirmwareData';

export default function App() {
  const [watchState, setWatchState] = useState(() => createInitialWatchState());
  const [workspaceTab, setWorkspaceTab] = useState<
    'SIMULATOR' | 'PINOUT' | 'FIRMWARE'
  >('SIMULATOR');
  const [scaleMode, setScaleMode] = useState<'1x' | '1.6x'>('1.6x');
  const [lastEventLabel, setLastEventLabel] = useState<string>('BOOT_READY');
  const [logs, setLogs] = useState<HardwareLogEntry[]>([
    {
      id: 'log-init-1',
      time: '22:28:00',
      event: 'SYSTEM',
      detail: 'ESP32-S3 (8MB PSRAM) + GC9A01 240x240 SPI DMA Initialized',
      screen: 'HOME',
    },
    {
      id: 'log-init-2',
      time: '22:28:01',
      event: 'SYSTEM',
      detail: 'OV2640 Camera & SD_MMC (/sdcard/DCIM, 4 files) Mounted',
      screen: 'HOME',
    },
  ]);

  // Optional live browser webcam stream for HandyCam mode
  const [webcamActive, setWebcamActive] = useState(false);
  const [webcamError, setWebcamError] = useState<string | null>(null);
  const hiddenVideoRef = useRef<HTMLVideoElement | null>(null);
  const mediaStreamRef = useRef<MediaStream | null>(null);

  // Sync active watch screen to document.body.dataset for automated inspection
  useEffect(() => {
    document.body.dataset.screen = watchState.screen;
  }, [watchState.screen]);

  const appendLog = useCallback(
    (
      event: HardwareLogEntry['event'],
      detail: string,
      screen: WatchScreenId
    ) => {
      const now = new Date();
      const time = `${String(now.getHours()).padStart(2, '0')}:${String(
        now.getMinutes()
      ).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`;
      setLogs((prev) => [
        {
          id: `log-${Date.now()}-${Math.random().toString(36).slice(2, 6)}`,
          time,
          event,
          detail,
          screen,
        },
        ...prev.slice(0, 24),
      ]);
    },
    []
  );

  const handleRotaryEvent = useCallback(
    (
      event: RotaryEvent,
      options?: { capturedDataUrl?: string; timestamp?: string }
    ) => {
      setLastEventLabel(event);
      setWatchState((prev) => {
        // Generate a snapshot dataUrl if user triggers SINGLE_CLICK on CAMERA SNAP from external button
        let opts = options;
        if (
          prev.screen === 'CAMERA' &&
          event === 'SINGLE_CLICK' &&
          CAMERA_CONTROLS[prev.cameraControlIndex].id === 'SNAP' &&
          !opts?.capturedDataUrl
        ) {
          const canvasEl = document.querySelector(
            '[data-testid="gc9a01-display"] canvas'
          ) as HTMLCanvasElement | null;
          if (canvasEl) {
            const now = new Date();
            const ts = `${now.getFullYear()}-${String(
              now.getMonth() + 1
            ).padStart(2, '0')}-${String(now.getDate()).padStart(
              2,
              '0'
            )} ${String(now.getHours()).padStart(2, '0')}:${String(
              now.getMinutes()
            ).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`;
            opts = {
              capturedDataUrl: canvasEl.toDataURL('image/jpeg', 0.92),
              timestamp: ts,
            };
          }
        }

        const next = reduceWatchState(prev, event, opts);

        // Log hardware event
        const detailMap: Record<RotaryEvent, string> = {
          CW: `ENC_CLK/DT Quadrature +1 (Pos ${next.encoderPosition})`,
          CCW: `ENC_CLK/DT Quadrature -1 (Pos ${next.encoderPosition})`,
          SINGLE_CLICK: `ENC_SW Short Press -> Select (${prev.screen} -> ${next.screen})`,
          DOUBLE_CLICK: `ENC_SW Double Press (<280ms) -> Menu Back (${prev.screen} -> ${next.screen})`,
        };
        appendLog(event, detailMap[event], next.screen);
        return next;
      });
    },
    [appendLog]
  );

  // Global keyboard bindings for testing & effortless operation of EC11 Rotary Switch
  useEffect(() => {
    const onKeyDown = (e: KeyboardEvent) => {
      const tag = (e.target as HTMLElement)?.tagName;
      if (tag === 'INPUT' || tag === 'TEXTAREA' || tag === 'SELECT') return;

      if (e.key === 'ArrowRight' || e.key === 'ArrowDown') {
        e.preventDefault();
        handleRotaryEvent('CW');
      } else if (e.key === 'ArrowLeft' || e.key === 'ArrowUp') {
        e.preventDefault();
        handleRotaryEvent('CCW');
      } else if (e.key === 'Enter') {
        if (tag !== 'BUTTON' && tag !== 'A') {
          e.preventDefault();
          handleRotaryEvent('SINGLE_CLICK');
        }
      } else if (e.key === 'Backspace' || e.key === 'Escape') {
        e.preventDefault();
        handleRotaryEvent('DOUBLE_CLICK');
      }
    };
    window.addEventListener('keydown', onKeyDown);
    return () => window.removeEventListener('keydown', onKeyDown);
  }, [handleRotaryEvent]);

  // Toggle live browser webcam feed for HandyCam
  const handleToggleWebcamLive = async () => {
    if (webcamActive) {
      if (mediaStreamRef.current) {
        mediaStreamRef.current.getTracks().forEach((t) => t.stop());
        mediaStreamRef.current = null;
      }
      setWebcamActive(false);
      setWatchState((prev) => ({ ...prev, cameraScene: 'LAB_BENCH' }));
      appendLog('SYSTEM', 'Browser Webcam disconnected; reverted to OV2640 Sim', watchState.screen);
      return;
    }

    try {
      setWebcamError(null);
      if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia) {
        throw new Error('MediaDevices API unavailable in this environment');
      }
      const stream = await navigator.mediaDevices.getUserMedia({
        video: { width: 240, height: 240 },
        audio: false,
      });
      mediaStreamRef.current = stream;
      if (hiddenVideoRef.current) {
        hiddenVideoRef.current.srcObject = stream;
        await hiddenVideoRef.current.play();
      }
      setWebcamActive(true);
      setWatchState((prev) => ({
        ...prev,
        screen: 'CAMERA',
        cameraScene: 'WEBCAM_LIVE',
        cameraStreaming: true,
      }));
      appendLog('SYSTEM', 'Connected live camera stream -> 240x240 HandyCam', 'CAMERA');
    } catch (err) {
      const msg = err instanceof Error ? err.message : 'Camera permission declined';
      setWebcamError(msg);
      setWatchState((prev) => ({
        ...prev,
        screen: 'CAMERA',
        cameraScene: 'LAB_BENCH',
        cameraStreaming: true,
      }));
    }
  };

  const handleDownloadFirmware = () => {
    const blob = new Blob([ESP32S3_ARDUINO_FIRMWARE], {
      type: 'text/plain;charset=utf-8',
    });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'esp32s3_gc9a01_smartwatch.ino';
    a.click();
    URL.revokeObjectURL(url);
  };

  return (
    <div className="min-h-screen bg-[#0B0E17] text-slate-100 flex flex-col">
      {/* Hidden video element for webcam stream capture */}
      <video
        ref={hiddenVideoRef}
        playsInline
        muted
        className="hidden"
        width={240}
        height={240}
      />

      {/* Strict 3-Zone Top Navigation Bar Contract */}
      <header className="flex items-center justify-between gap-8 px-6 py-4 border-b border-slate-800/90 bg-[#0B0E17]">
        {/* Zone 1: Single text element wordmark */}
        <a
          href="#top"
          onClick={(e) => {
            e.preventDefault();
            setWorkspaceTab('SIMULATOR');
          }}
          className="text-lg font-bold tracking-tight text-white whitespace-nowrap shrink-0"
        >
          ChronoS3
        </a>

        {/* Zone 2: 5 concise single-line text navigation links */}
        <nav className="hidden md:flex items-center gap-6 text-sm font-medium text-slate-300">
          <a
            href="#simulator"
            onClick={(e) => {
              e.preventDefault();
              setWorkspaceTab('SIMULATOR');
            }}
            className={`hover:text-white transition-colors whitespace-nowrap shrink-0 ${
              workspaceTab === 'SIMULATOR' ? 'text-cyan-400 underline underline-offset-8' : ''
            }`}
          >
            Simulator
          </a>
          <a
            href="#handycam"
            onClick={(e) => {
              e.preventDefault();
              setWorkspaceTab('SIMULATOR');
              setWatchState((prev) => ({ ...prev, screen: 'CAMERA' }));
            }}
            className="hover:text-white transition-colors whitespace-nowrap shrink-0"
          >
            HandyCam
          </a>
          <a
            href="#gallery"
            onClick={(e) => {
              e.preventDefault();
              setWorkspaceTab('SIMULATOR');
              setWatchState((prev) => ({
                ...prev,
                screen: 'GALLERY',
                galleryMode: 'GRID',
              }));
            }}
            className="hover:text-white transition-colors whitespace-nowrap shrink-0"
          >
            SD Gallery
          </a>
          <a
            href="#pinout"
            onClick={(e) => {
              e.preventDefault();
              setWorkspaceTab('PINOUT');
            }}
            className={`hover:text-white transition-colors whitespace-nowrap shrink-0 ${
              workspaceTab === 'PINOUT' ? 'text-cyan-400 underline underline-offset-8' : ''
            }`}
          >
            Pinout
          </a>
          <a
            href="#firmware"
            onClick={(e) => {
              e.preventDefault();
              setWorkspaceTab('FIRMWARE');
            }}
            className={`hover:text-white transition-colors whitespace-nowrap shrink-0 ${
              workspaceTab === 'FIRMWARE' ? 'text-cyan-400 underline underline-offset-8' : ''
            }`}
          >
            Firmware
          </a>
        </nav>

        {/* Zone 3: 1 primary action */}
        <div className="flex items-center gap-3 shrink-0">
          <button
            type="button"
            onClick={handleDownloadFirmware}
            className="px-4 py-2 text-xs font-semibold text-slate-950 bg-cyan-400 rounded-lg hover:bg-cyan-300 transition-colors whitespace-nowrap shrink-0 cursor-pointer"
          >
            Download .ino
          </button>
        </div>
      </header>

      {/* Main Engineering Workspace */}
      <main className="flex-1 max-w-[1360px] w-full mx-auto px-4 sm:px-6 py-4 flex flex-col gap-4">
        {/* Workspace Toolbar & Telemetry Header */}
        <div className="flex flex-col lg:flex-row lg:items-center justify-between gap-3 pb-3 border-b border-slate-800/80">
          <div>
            <div className="flex flex-wrap items-center gap-2 text-xs font-mono text-slate-400 mb-1">
              <span className="whitespace-nowrap">ESP32-S3-WROOM-1 (240MHz Dual-Core)</span>
              <span aria-hidden="true">·</span>
              <span className="whitespace-nowrap">GC9A01 240×240 Round IPS</span>
              <span aria-hidden="true">·</span>
              <span className="whitespace-nowrap">OV2640 DVP Camera</span>
              <span aria-hidden="true">·</span>
              <span className="whitespace-nowrap">EC11 Rotary Encoder</span>
            </div>
            <h1 className="text-xl sm:text-2xl font-bold text-white tracking-tight">
              ESP32-S3 Round Smart Watch & HandyCam Hardware Studio
            </h1>
          </div>

          {/* Interactive Mode Switcher (Simulator / Pinout / Firmware) */}
          <div className="flex flex-wrap items-center gap-2">
            <div className="flex items-center gap-1 p-1 bg-slate-900 border border-slate-800 rounded-lg">
              <button
                type="button"
                onClick={() => setWorkspaceTab('SIMULATOR')}
                className={`px-3 py-1.5 text-xs font-medium rounded-md transition-colors cursor-pointer whitespace-nowrap shrink-0 ${
                  workspaceTab === 'SIMULATOR'
                    ? 'bg-cyan-500 text-slate-950 font-semibold'
                    : 'text-slate-300 hover:text-white'
                }`}
              >
                Watch & Rotary Rig
              </button>
              <button
                type="button"
                onClick={() => setWorkspaceTab('PINOUT')}
                className={`px-3 py-1.5 text-xs font-medium rounded-md transition-colors cursor-pointer whitespace-nowrap shrink-0 ${
                  workspaceTab === 'PINOUT'
                    ? 'bg-cyan-500 text-slate-950 font-semibold'
                    : 'text-slate-300 hover:text-white'
                }`}
              >
                GPIO Wiring Matrix
              </button>
              <button
                type="button"
                onClick={() => setWorkspaceTab('FIRMWARE')}
                className={`px-3 py-1.5 text-xs font-medium rounded-md transition-colors cursor-pointer whitespace-nowrap shrink-0 ${
                  workspaceTab === 'FIRMWARE'
                    ? 'bg-cyan-500 text-slate-950 font-semibold'
                    : 'text-slate-300 hover:text-white'
                }`}
              >
                Arduino Files (.ino + .h)
              </button>
            </div>

            {workspaceTab === 'SIMULATOR' && (
              <div className="flex items-center gap-1 p-1 bg-slate-900 border border-slate-800 rounded-lg">
                <button
                  type="button"
                  onClick={() => setScaleMode('1x')}
                  className={`px-2.5 py-1.5 text-xs font-mono rounded-md transition-colors cursor-pointer whitespace-nowrap shrink-0 ${
                    scaleMode === '1x'
                      ? 'bg-slate-700 text-white font-semibold'
                      : 'text-slate-400 hover:text-slate-200'
                  }`}
                >
                  1× (240px)
                </button>
                <button
                  type="button"
                  onClick={() => setScaleMode('1.6x')}
                  className={`px-2.5 py-1.5 text-xs font-mono rounded-md transition-colors cursor-pointer whitespace-nowrap shrink-0 ${
                    scaleMode === '1.6x'
                      ? 'bg-slate-700 text-white font-semibold'
                      : 'text-slate-400 hover:text-slate-200'
                  }`}
                >
                  1.6× Bench Zoom
                </button>
              </div>
            )}
          </div>
        </div>

        {/* Main Active View */}
        {workspaceTab === 'SIMULATOR' ? (
          <div className="grid grid-cols-1 lg:grid-cols-12 gap-5 items-start">
            {/* Left Column: 240x240 GC9A01 Round Watch Hardware + EC11 Rotary Switch */}
            <div className="lg:col-span-5 flex flex-col items-center gap-4">
              <div className="w-full bg-[#111827] border border-slate-800 rounded-xl p-4 flex flex-col items-center">
                <div className="w-full flex items-center justify-between text-xs font-mono text-slate-400 mb-2">
                  <span>GC9A01 · 240×240 RGB565</span>
                  <span className="text-cyan-400">
                    SCREEN: {watchState.screen}
                  </span>
                </div>

                {/* Circular 240x240 Display Bezel */}
                <GC9A01RoundDisplay
                  state={watchState}
                  onRotaryEvent={handleRotaryEvent}
                  onDirectSelectMenu={(menuIdx) => {
                    setWatchState((prev) => ({
                      ...prev,
                      menuIndex: menuIdx,
                    }));
                  }}
                  onUpdateScore={(score) => {
                    setWatchState((prev) => ({
                      ...prev,
                      gameScore: score,
                      gameBestScore: Math.max(prev.gameBestScore, score),
                    }));
                  }}
                  liveVideoElement={hiddenVideoRef.current}
                  scaleMode={scaleMode}
                />

                <div className="w-full mt-3 pt-2.5 border-t border-slate-800/80 flex items-center justify-between text-xs font-mono text-slate-400">
                  <span>1×Click: Select</span>
                  <span>·</span>
                  <span>2×Click: Menu Back</span>
                  <span>·</span>
                  <span>Rotate: Scroll</span>
                </div>
              </div>

              {/* Physical EC11 Rotary Switch Deck */}
              <div className="w-full">
                <RotarySwitchDeck
                  state={watchState}
                  onRotaryEvent={handleRotaryEvent}
                  lastEventLabel={lastEventLabel}
                />
              </div>
            </div>

            {/* Right Column: HandyCam Stream Source, SD Card (/sdcard/DCIM) Reader, & Serial Trace */}
            <div className="lg:col-span-7">
              <HardwarePeripheralPanel
                state={watchState}
                onJumpToScreen={(targetScreen) => {
                  setWatchState((prev) => ({
                    ...prev,
                    screen: targetScreen,
                    galleryMode:
                      targetScreen === 'GALLERY' ? 'GRID' : prev.galleryMode,
                    gameMode:
                      targetScreen === 'GAMES' ? 'SELECT' : prev.gameMode,
                  }));
                  appendLog(
                    'SYSTEM',
                    `Direct Screen Jump -> ${targetScreen}`,
                    targetScreen
                  );
                }}
                onSelectCameraScene={(scene: CameraScene) => {
                  setWatchState((prev) => ({
                    ...prev,
                    screen: 'CAMERA',
                    cameraScene: scene,
                    cameraStreaming: true,
                  }));
                  appendLog(
                    'SYSTEM',
                    `OV2640 Camera Feed -> ${scene}`,
                    'CAMERA'
                  );
                }}
                onSelectCameraFilter={(filter: CameraFilter) => {
                  setWatchState((prev) => ({
                    ...prev,
                    screen: 'CAMERA',
                    cameraFilter: filter,
                  }));
                  appendLog('SYSTEM', `OV2640 ISP Filter -> ${filter}`, 'CAMERA');
                }}
                onToggleWebcamLive={handleToggleWebcamLive}
                webcamActive={webcamActive}
                webcamError={webcamError}
                onViewSdImageOnWatch={(idx) => {
                  setWatchState((prev) => ({
                    ...prev,
                    screen: 'GALLERY',
                    galleryMode: 'VIEWER',
                    galleryIndex: idx,
                    galleryShowExif: true,
                  }));
                  appendLog(
                    'SYSTEM',
                    `Opened /sdcard/DCIM/${watchState.sdImages[idx]?.filename} on GC9A01`,
                    'GALLERY'
                  );
                }}
                onDeleteSdImage={(id) => {
                  setWatchState((prev) => {
                    const filtered = prev.sdImages.filter((item) => item.id !== id);
                    const nextIdx = Math.min(
                      prev.galleryIndex,
                      Math.max(0, filtered.length - 1)
                    );
                    return {
                      ...prev,
                      sdImages: filtered,
                      galleryIndex: nextIdx,
                    };
                  });
                  appendLog('SD_WRITE', `Unlinked file ${id} from /sdcard/DCIM`, watchState.screen);
                }}
                logs={logs}
              />
            </div>
          </div>
        ) : (
          <FirmwareAndPinoutSection activeTab={workspaceTab} />
        )}
      </main>
    </div>
  );
}
