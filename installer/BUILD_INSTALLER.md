# How to Build the Installer

## Prerequisites

1. Install **Inno Setup 6** (free): https://jrsoftware.org/isdl.php
2. Build the plugin in Release mode first (see README.md)

## Optional: Add a custom icon

- Place a 256x256 `.ico` file at `installer/assets/icon.ico`
- If you don't have one, remove the `SetupIconFile=` line from the .iss script

## Steps

### 1. Build the plugin

```
cd d:\plugins\GuitarPedalRack
cmake --build build --config Release -j8
```

### 2. Verify these files exist:

- `build\GuitarPedalRack_artefacts\Release\Standalone\Aether Guitar Pedal Rack.exe`
- `build\GuitarPedalRack_artefacts\Release\VST3\Aether Guitar Pedal Rack.vst3\` (folder)

### 3. Build the installer

**Option A - GUI:**

- Open Inno Setup Compiler
- File → Open → select `installer\AetherGuitarPedalRack.iss`
- Build → Compile (Ctrl+F9)

**Option B - Command line:**

```
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" installer\AetherGuitarPedalRack.iss
```

### 4. Find the output

The installer will be created at:

```
installer\output\AetherGuitarPedalRack_v1.0.0_Setup.exe
```

## What the installer does

- Installs standalone app to `C:\Program Files\Aether Audio\Aether Guitar Pedal Rack\`
- Installs VST3 to `C:\Program Files\Common Files\VST3\`
- Creates Start Menu shortcuts
- Optional: Desktop shortcut
- Creates user preset folder at `%LOCALAPPDATA%\GuitarPedalRack\Presets\`
- Includes a full uninstaller

## Notes

- Requires admin rights (needed to write to Program Files and VST3 folder)
- User presets stored in `%LOCALAPPDATA%\GuitarPedalRack\Presets\` are NOT deleted on uninstall
- The AppId GUID is unique to this plugin — do not change it between versions
