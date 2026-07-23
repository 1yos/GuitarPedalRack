# 🎸 Aether Guitar Pedal Rack

[![Build](https://img.shields.io/badge/build-passing-brightgreen)]()
[![C++17](https://img.shields.io/badge/C++-17-blue)]()
[![JUCE 7](https://img.shields.io/badge/JUCE-7.x-orange)]()
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS-lightgrey)]()
[![Version](https://img.shields.io/badge/version-1.0.0-gold)]()

> **Aether Guitar Pedal Rack** is a professional boutique guitar pedalboard VST3/Standalone plugin with 100+ studio-quality effects, photorealistic pedal rendering, and intelligent preset management.

---

## Features

### 100+ Effects

- **Drive** — Tube Overdrive, Klon Centaur, TS808/TS9, Big Muff, ProCo RAT, Boss DS-1/SD-1, Blues Driver, Metal Zone, Fuzz Face, Timmy, and more
- **Dynamics** — Noise Gate, Compressor (FET/VCA/Optical/Tube), Limiter, Expander, Multiband Compressor, De-Esser
- **Modulation** — Chorus, Ensemble, Phaser, Flanger, Tremolo, Vibrato, Rotary Speaker, Ring Modulator
- **Delay** — Digital, Analog, Tape, Ping-Pong, Multi-Tap, Reverse, Slapback, Dotted, Dual, Memory Man, Echoplex, Space Echo
- **Reverb** — Plate, Spring, Hall, Room, Chamber, Shimmer, Reverse, Gated, Modulated
- **Filter** — Wah, Auto-Wah, Envelope Filter, Low/High/Band Pass, Notch, Formant, State Variable, Talk Box
- **Pitch** — Pitch Shifter, Harmonizer, Octaver Up/Down, Whammy, Detune, Pitch Vibrato
- **EQ** — Parametric, Graphic 10/31-Band, Shelving, Tone Stack (Fender/Marshall)
- **Amp & Cabinet** — 3-channel amp simulator, Cabinet IR

### Premium UI

- Photorealistic boutique pedal rendering (powder-coated metal, brushed aluminum, glass LEDs)
- Drag-and-drop signal chain reordering
- Two-row pedalboard supporting up to 10+ pedals
- Animated patch cables with signal flow visualization
- Real-time parameter editor with waveform visualization

### Preset System

- 70 factory presets across 6 categories (Clean, Rock, Metal, Blues, Ambient, Lead)
- Save/load user presets with full parameter state
- Category filtering and search
- Active preset indicator

---

## System Requirements

|           | Minimum                  | Recommended             |
| --------- | ------------------------ | ----------------------- |
| **OS**    | Windows 10 / macOS 10.13 | Windows 11 / macOS 13   |
| **CPU**   | Intel i5 / AMD Ryzen 5   | Intel i7 / AMD Ryzen 7  |
| **RAM**   | 4 GB                     | 8 GB                    |
| **Audio** | ASIO/CoreAudio interface | Low-latency interface   |
| **DAW**   | VST3 compatible          | Reaper, Live, FL Studio |

---

## Installation

### Standalone

1. Build or download the executable
2. Run `Aether Guitar Pedal Rack.exe` (Windows) or `.app` (macOS)

### VST3

1. Build the VST3 target
2. Copy the `.vst3` bundle to your VST3 folder:
   - **Windows:** `C:\Program Files\Common Files\VST3\`
   - **macOS:** `~/Library/Audio/Plug-Ins/VST3/`
3. Rescan plugins in your DAW

---

## Build from Source

### Requirements

- CMake 3.22+
- MSVC 2019+ (Windows) / Xcode 11+ (macOS)
- JUCE 7.x (included as submodule dependency)

### Windows

```powershell
git clone https://github.com/1yos/GuitarPedalRack.git
cd GuitarPedalRack
cmake -B build
cmake --build build --config Release --target GuitarPedalRack_Standalone -j8
```

### macOS

```bash
git clone https://github.com/1yos/GuitarPedalRack.git
cd GuitarPedalRack
cmake -B build
cmake --build build --config Release --target GuitarPedalRack_Standalone -j8
```

---

## Usage

1. **Add effects** — Click `ADD EFFECT` to open the browser, pick a pedal
2. **Adjust parameters** — Click a pedal to select it, use the bottom panel knobs
3. **Bypass** — Click the footswitch on any pedal
4. **Reorder** — Drag pedals left/right to change signal chain order
5. **Presets** — Click `PRESETS` to browse 70 factory presets or save your own

---

## Performance

| Chain     | CPU @ 44.1kHz/512 buffer |
| --------- | ------------------------ |
| Empty     | < 1%                     |
| 4 pedals  | 5–10%                    |
| 10 pedals | 12–20%                   |

---

## Credits

Built with [JUCE](https://juce.com/) · Inspired by Neural DSP, Strymon, Universal Audio

---

## License

MIT — free to use, modify, and distribute. Attribution appreciated.
