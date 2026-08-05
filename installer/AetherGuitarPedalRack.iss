; ============================================================
;  Aether Guitar Pedal Rack - Inno Setup Installer Script
;  Version 1.0.0
;  Author: Aether Audio
;  https://github.com/1yos/GuitarPedalRack
; ============================================================

#define AppName      "Aether Guitar Pedal Rack"
#define AppVersion   "1.0.0"
#define AppPublisher "Aether Audio"
#define AppURL       "https://github.com/1yos/GuitarPedalRack"
#define AppExeName   "Aether Guitar Pedal Rack.exe"

[Setup]
AppId={{A3B7C2D1-9E4F-4A8B-B5C6-D7E8F9A0B1C2}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}
AppUpdatesURL={#AppURL}
DefaultDirName={autopf}\Aether Audio\Aether Guitar Pedal Rack
DefaultGroupName={#AppName}
AllowNoIcons=yes
LicenseFile=license.txt
OutputDir=output
OutputBaseFilename=AetherGuitarPedalRack_v{#AppVersion}_Setup
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
WizardSizePercent=130
PrivilegesRequired=admin
UninstallDisplayIcon={app}\{#AppExeName}
UninstallDisplayName={#AppName} v{#AppVersion}
VersionInfoVersion={#AppVersion}
VersionInfoCompany={#AppPublisher}
VersionInfoDescription={#AppName} Installer
VersionInfoCopyright=Copyright (C) 2026 {#AppPublisher}
MinVersion=6.1sp1

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional icons:"; Flags: unchecked

[Files]
; ── Standalone Application ───────────────────────────────────────────────────
Source: "..\build\GuitarPedalRack_artefacts\Release\Standalone\Aether Guitar Pedal Rack.exe"; \
    DestDir: "{app}"; \
    Flags: ignoreversion

; ── VST3 Plugin ──────────────────────────────────────────────────────────────
Source: "..\build\GuitarPedalRack_artefacts\Release\VST3\Aether Guitar Pedal Rack.vst3\*"; \
    DestDir: "{commoncf}\VST3\Aether Guitar Pedal Rack.vst3"; \
    Flags: ignoreversion recursesubdirs createallsubdirs

; ── Documentation ────────────────────────────────────────────────────────────
Source: "..\README.md"; \
    DestDir: "{app}"; \
    DestName: "README.md"; \
    Flags: ignoreversion isreadme

[Dirs]
; Create user preset folder on install
Name: "{localappdata}\GuitarPedalRack\Presets"

[Icons]
Name: "{group}\{#AppName}"; \
    Filename: "{app}\{#AppExeName}"; \
    WorkingDir: "{app}"
Name: "{group}\Uninstall {#AppName}"; \
    Filename: "{uninstallexe}"
Name: "{autodesktop}\{#AppName}"; \
    Filename: "{app}\{#AppExeName}"; \
    Tasks: desktopicon

[Run]
Filename: "{app}\{#AppExeName}"; \
    Description: "Launch {#AppName} now"; \
    Flags: nowait postinstall skipifsilent

[UninstallDelete]
; Remove VST3 on uninstall (user presets in %LOCALAPPDATA% are preserved)
Type: filesandordirs; Name: "{commoncf}\VST3\Aether Guitar Pedal Rack.vst3"

[Code]
function InitializeSetup(): Boolean;
var
  PrevVersion: String;
begin
  Result := True;
  if RegQueryStringValue(HKLM,
    'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{A3B7C2D1-9E4F-4A8B-B5C6-D7E8F9A0B1C2}_is1',
    'DisplayVersion', PrevVersion) then
  begin
    if MsgBox(
      'Aether Guitar Pedal Rack v' + PrevVersion + ' is already installed.' + #13#10 + #13#10 +
      'This will upgrade to v{#AppVersion}. Your user presets will be kept.' + #13#10 + #13#10 +
      'Continue with the upgrade?',
      mbConfirmation, MB_YESNO) = IDNO then
      Result := False;
  end;
end;
