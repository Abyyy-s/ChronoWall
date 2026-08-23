# 🕒 ChronoWall

> **GNOME-style dynamic wallpapers for Cinnamon and KDE Plasma desktops.**
>
> A lightweight C++17 daemon that turns dynamic wallpaper timelines into time-aware desktop backgrounds — without a permanent rendering window or a continuous rendering loop.

<p align="center">
  <a href="https://github.com/Abyyy-s/ChronoWall/releases"><strong>📦 Releases</strong></a> ·
  <a href="https://github.com/Abyyy-s/ChronoWall-Wallpapers"><strong>🖼️ Wallpaper Collection</strong></a> ·
  <a href="https://github.com/Abyyy-s/ChronoWall/issues"><strong>🐛 Issues</strong></a>
</p>

---

## ✨ What is ChronoWall?

ChronoWall reads **GNOME dynamic wallpaper XML files**, determines which timeline event should currently be active, and applies the corresponding image to **Cinnamon** (via `gsettings`) or **KDE Plasma** (via `plasma-apply-wallpaperimage`).

Instead of continuously rendering the desktop, ChronoWall changes the wallpaper when the timeline changes and then **sleeps until the next event boundary**.

### Why ChronoWall?

- ⚡ Lightweight C++17 daemon
- 🕐 Time-aware dynamic wallpaper scheduling
- 🖼️ GNOME-style XML compatibility
- 🧩 Relative image-path support
- 🖥️ Native Cinnamon & KDE Plasma (Wayland / X11) wallpaper integration
- 🔋 Very low idle resource usage (sleeps between event boundaries)
- ⚙️ systemd user-service integration
- 🚫 No permanent rendering window
- 🚫 No continuous GPU/rendering loop
- ⌨️ Simple CLI

---

# 🎨 See the Wallpapers in Action

The companion **[ChronoWall Wallpapers](https://github.com/Abyyy-s/ChronoWall-Wallpapers)** repository contains the full-resolution wallpaper collection and animated previews.

These are real animated previews from the collection — they play directly on GitHub.

### 🌄 Landscapes

| Adwaita | Big Sur | Mojave | Magic Lake |
|---|---|---|---|
| ![Adwaita](https://raw.githubusercontent.com/Abyyy-s/ChronoWall-Wallpapers/main/previews/Adwaita.gif) | ![Big Sur](https://raw.githubusercontent.com/Abyyy-s/ChronoWall-Wallpapers/main/previews/BigSur.gif) | ![Mojave](https://raw.githubusercontent.com/Abyyy-s/ChronoWall-Wallpapers/main/previews/Mojave.gif) | ![Magic Lake](https://raw.githubusercontent.com/Abyyy-s/ChronoWall-Wallpapers/main/previews/MagicLake.gif) |

### 🌃 Lofi & Digital

| Lofi Girl | Tokyo Street | Cyberpunk | Zorin Blur |
|---|---|---|---|
| ![Lofi Girl](https://raw.githubusercontent.com/Abyyy-s/ChronoWall-Wallpapers/main/previews/LofiGirl.gif) | ![Tokyo Street](https://raw.githubusercontent.com/Abyyy-s/ChronoWall-Wallpapers/main/previews/TokyoStreet.gif) | ![Cyberpunk](https://raw.githubusercontent.com/Abyyy-s/ChronoWall-Wallpapers/main/previews/cyberpunk-01.gif) | ![Zorin Blur](https://raw.githubusercontent.com/Abyyy-s/ChronoWall-Wallpapers/main/previews/ZorinBlur.gif) |

👉 **[Browse all 109 wallpapers →](https://github.com/Abyyy-s/ChronoWall-Wallpapers)**

---

# 🚀 Installation

## Requirements

ChronoWall supports **Linux Mint / Cinnamon** and **KDE Plasma (Wayland / X11)** on Linux distributions.

On **Debian / Ubuntu / Linux Mint**:
```bash
sudo apt install build-essential cmake libtinyxml2-dev
```

On **Fedora**:
```bash
sudo dnf install gcc-c++ cmake tinyxml2-devel plasma-workspace
```

`gsettings` is standard on Cinnamon desktops, and `plasma-apply-wallpaperimage` is provided by `plasma-workspace` on KDE Plasma.

## 1. Clone ChronoWall

```bash
git clone https://github.com/Abyyy-s/ChronoWall.git
cd ChronoWall
```

## 2. Choose a Dynamic Wallpaper

The easiest way to get wallpapers is the companion collection:

👉 **[ChronoWall Wallpapers](https://github.com/Abyyy-s/ChronoWall-Wallpapers/releases)**

Download the latest wallpaper archive and extract it.

A wallpaper is organized like this:

```text
wallpapers/
└── StepbyStep/
    ├── StepbyStep.xml
    └── StepbyStep/
        ├── StepbyStep-1.png
        └── StepbyStep-2.png
```

## 3. Install ChronoWall with a Wallpaper

From the ChronoWall source directory:

```bash
./install.sh /path/to/wallpaper.xml
```

Example:

```bash
./install.sh ~/Projects/ChronoWall-Wallpapers/wallpapers/StepbyStep/StepbyStep.xml
```

The installer:

1. Builds a Release binary with CMake.
2. Installs `chronowall` into `~/.local/bin`.
3. Copies the selected XML to `~/.config/chronowall/wallpaper.xml`.
4. Creates a systemd user service.
5. Enables and starts the service.

Make sure `~/.local/bin` is in your `PATH`.

---

# ⌨️ CLI

```bash
chronowall --help
chronowall --version
chronowall status
chronowall start
chronowall stop
chronowall restart
```

For testing or debugging a wallpaper directly:

```bash
chronowall run /path/to/wallpaper.xml
```

The legacy shorthand is also supported:

```bash
chronowall /path/to/wallpaper.xml
```

---

# 🛠️ Service & Logs

ChronoWall runs as a **systemd user service**, so `sudo` is not required to manage it.

```bash
systemctl --user status chronowall.service
```

Follow live logs with:

```bash
journalctl --user -u chronowall.service -f
```

---

# 🧠 How It Works

```text
┌──────────────────────────────┐
│ GNOME Dynamic Wallpaper XML  │
└──────────────┬───────────────┘
               ↓
       ┌───────────────┐
       │ WallpaperParser│
       └───────┬───────┘
               ↓
       ┌───────────────┐
       │    Timeline   │
       └───────┬───────┘
               ↓
       ┌───────────────┐
       │ Wallpaper     │
       │ Scheduler     │
       └───────┬───────┘
               ↓
       ┌───────────────┐
       │ Wallpaper     │
       │ Changer       │
       └───────┬───────┘
               ↓
       ┌───────────────┐
       │ Wallpaper     │
       │ BackendFactory│
       └───┬───────┬───┘
           │       │
           ▼       ▼
    Cinnamon       KDE Plasma
  (gsettings)    (plasma-apply-wallpaperimage)
```

The runtime path is intentionally clean and modular:

```text
XML → Parser → Scheduler → WallpaperChanger → Desktop Backend → Desktop
```

ChronoWall evaluates the current event, applies the appropriate image, and sleeps until the next event boundary.

---

# 📄 XML Support

ChronoWall understands the important timeline elements used by GNOME dynamic wallpapers, including:

- `<starttime>`
- `<static>`
- `<transition>`
- Event durations
- Relative image paths

For example:

```xml
<static>
    <file>StepbyStep/StepbyStep-1.png</file>
    <duration>5.0</duration>
</static>

<transition type="overlay">
    <duration>5.0</duration>
    <from>StepbyStep/StepbyStep-1.png</from>
    <to>StepbyStep/StepbyStep-2.png</to>
</transition>
```

Relative paths are resolved from the directory containing the XML file, allowing a wallpaper package to remain self-contained.

---

# ⚠️ About Transitions in v1.0.0

ChronoWall v1 preserves the **timing and structure** of transition events, but the stable v1 implementation does **not** perform a true pixel-by-pixel crossfade.

For a transition event, ChronoWall applies the destination image through the desktop's native wallpaper API and waits for the transition duration before advancing to the next timeline event.

Experimental live rendering work is kept separate from the stable v1 implementation.

---

# 🏗️ Architecture

The modular architecture cleanly decouples the core engine from desktop-specific integration:

```text
WallpaperParser
      ↓
DynamicWallpaper
      ↓
TimelineEvent
      ↓
WallpaperScheduler
      ↓
WallpaperChanger
      ↓
WallpaperBackend (Interface)
  ├── CinnamonBackend (gsettings)
  └── KDEPlasmaBackend (plasma-apply-wallpaperimage)
```

Core components include:

- `WallpaperParser`: Parses GNOME XML and resolves image paths.
- `DynamicWallpaper`: Data model for frames, transitions, and timeline.
- `WallpaperFrame`: Individual frame metadata.
- `Transition`: Transition event definition.
- `TimelineEvent`: 24-hour timeline event scheduler unit.
- `WallpaperScheduler`: Desktop-independent timeline calculator.
- `WallpaperBackend`: Abstract desktop wallpaper backend interface.
- `CinnamonBackend`: Cinnamon desktop integration via `gsettings`.
- `KDEPlasmaBackend`: KDE Plasma (Wayland & X11) desktop integration via `plasma-apply-wallpaperimage`.
- `WallpaperBackendFactory`: Deterministic backend detection and instantiation.
- `WallpaperChanger`: High-level wallpaper coordinator.

---

# 🧪 Development

Build manually with CMake:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

Run directly:

```bash
./build/ChronoWall run /path/to/wallpaper.xml
```

---

# 🗑️ Uninstall

From the repository:

```bash
./uninstall.sh
```

This stops and disables the service and removes the ChronoWall executable, service file, and configuration directory.

---

# 🐛 Troubleshooting

### Check whether ChronoWall is running

```bash
chronowall status
```

### Follow daemon logs

```bash
journalctl --user -u chronowall.service -f
```

### Stop it immediately

```bash
chronowall stop
```

### Rebuild from scratch

```bash
rm -rf build build-release
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

### Backend Selection & Override

ChronoWall automatically detects whether it is running under **KDE Plasma** or **Cinnamon** using standard session environment variables (`XDG_CURRENT_DESKTOP`, `XDG_SESSION_DESKTOP`, `DESKTOP_SESSION`).

To explicitly force a specific backend, set the `CHRONOWALL_BACKEND` environment variable:

```bash
# Force KDE Plasma backend
CHRONOWALL_BACKEND=kde chronowall run /path/to/wallpaper.xml

# Force Cinnamon backend
CHRONOWALL_BACKEND=cinnamon chronowall run /path/to/wallpaper.xml
```

---

# 🗺️ Current Scope

| Area | Status |
|---|---|
| Linux (Fedora, Debian, Ubuntu, Mint, Arch) | ✅ |
| Cinnamon Desktop (X11) | ✅ |
| KDE Plasma 6 / 5 (Wayland & X11) | ✅ |
| GNOME-style XML timelines | ✅ |
| Time-based scheduling | ✅ |
| Relative wallpaper paths | ✅ |
| systemd user service | ✅ |
| Low idle resource usage | ✅ |
| Multi-monitor synchronized wallpapers | ✅ |
| True live pixel crossfade | 🚧 Experimental |
| Other desktop environments | 🔮 Future |
| Windows support | 🔮 Future |

---

# 📦 Wallpaper Collection

Looking for wallpapers rather than the engine?

### 👉 [ChronoWall Wallpapers](https://github.com/Abyyy-s/ChronoWall-Wallpapers)

The companion repository contains:

- **109 dynamic wallpaper definitions**
- **109 XML configurations**
- **107 animated preview GIFs**
- Full-resolution wallpaper assets
- Attribution and source information
- A downloadable v1.0.0 collection archive

---

# 🤝 Contributing

Found a bug, broken XML file, or compatibility issue?

Open an issue or submit a pull request with a clear description of the change.

For wallpaper contributions, please include appropriate attribution and licensing information.

---

# 📜 License

ChronoWall is released under the **MIT License**.

See [`LICENSE`](LICENSE) for details.

---

<p align="center">
  <strong>🕒 ChronoWall</strong><br>
  Dynamic wallpapers, scheduled beautifully on Linux.
</p>
