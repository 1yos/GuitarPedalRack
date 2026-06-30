# 🚀 Quick Launch Instructions

## Run the Plugin Now

**Executable Location:**

```
d:\plugins\GuitarPedalRack\build\GuitarPedalRack_artefacts\Release\Standalone\Guitar Pedal Rack.exe
```

**Double-click the .exe file to launch!**

---

## What You'll See

### **Window Size:** 1280 x 720 pixels (fixed)

### **Layout:**

```
┌─────────────────────────────────────────────────────────────┐
│  GUITAR PEDAL RACK                           CPU: XX%      │
├─────────────────────────────────────────────────────────────┤
│  IN  │                                              │  OUT  │
│  │   │       🎛️ ALPHA AMP (Gold)                  │   │   │
│  │   │       Gain Bass Mid Treble Presence Master  │   │   │
│  │   │       [Channel: Clean/Crunch/Lead]          │   │   │
│  ↕   │                                              │   ↕   │
│      │                                              │       │
│──────┼──────────────────────────────────────────────┼───────│
│      │                                              │       │
│      │  🟠 DEEP HEAT    🟣 PULSE      🔵 VOID     │       │
│      │  [Drive][Tone]   [Rate][Depth] [Size][Decay]│       │
│      │    [Level]         [Mix]         [Mix]       │       │
│      │   [BYPASS]       [BYPASS]      [BYPASS]     │       │
└─────────────────────────────────────────────────────────────┘
```

---

## How to Use

### **Pedals (Bottom Section)**

- **Click & Drag Knobs** vertically to adjust
- **Click BYPASS** buttons to turn effects on/off
- **Watch LEDs** - Glow when active, dark when bypassed

### **Amp (Middle Section)**

- **Select Channel** from dropdown (Clean/Crunch/Lead)
- **Adjust EQ** - Gain, Bass, Mid, Treble, Presence
- **Set Master Volume** - Final amp output level

### **Global Controls**

- **Input Fader** (left side) - Drive pedals harder
- **Output Fader** (right side) - Final output level
- **CPU Monitor** (top right) - Performance indicator

---

## Signal Flow

```
Guitar Input → [Input Gain] →
  Noise Gate →
  🟠 DEEP HEAT (Overdrive) →
  🟣 PULSE (Chorus) →
  🎛️ ALPHA AMP →
  🔵 VOID (Reverb) →
  Cabinet IR →
[Output Gain] → Audio Output
```

---

## Quick Presets (Manual Settings)

### **Clean Ambient**

- Deep Heat: Bypassed
- Pulse: Rate 0.8, Depth 0.6, Mix 0.4
- Void: Size 0.8, Decay 0.7, Mix 0.5
- Amp: Channel Clean, Gain 3, Master 7

### **Crunch Rock**

- Deep Heat: Drive 0.6, Tone 0.5, Level 0.7
- Pulse: Bypassed
- Void: Size 0.3, Decay 0.4, Mix 0.2
- Amp: Channel Crunch, Gain 6, Master 6

### **Lead Solo**

- Deep Heat: Drive 0.8, Tone 0.7, Level 0.8
- Pulse: Rate 2.0, Depth 0.3, Mix 0.3
- Void: Size 0.5, Decay 0.6, Mix 0.3
- Amp: Channel Lead, Gain 8, Master 7

---

## Troubleshooting

### **Plugin doesn't launch**

- Check file location is correct
- Ensure no antivirus blocking
- Run as administrator if needed

### **No sound**

- Check audio device settings in plugin menu
- Verify input source is selected
- Ensure output volume is up

### **Controls not responding**

- Click and drag vertically on knobs
- Ensure window has focus
- Try restarting the plugin

---

## VST3 Installation (For DAW Use)

**VST3 Location:**

```
d:\plugins\GuitarPedalRack\build\GuitarPedalRack_artefacts\Release\VST3\Guitar Pedal Rack.vst3
```

**To Install:**

1. Copy the `.vst3` folder to: `C:\Program Files\Common Files\VST3\`
   (Requires administrator rights)
2. Restart your DAW
3. Scan for new plugins
4. Look for "Guitar Pedal Rack" in your effects list

---

## Status: ✅ PRODUCTION READY

All features are fully functional and connected. Enjoy! 🎸
