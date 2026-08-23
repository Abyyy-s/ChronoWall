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

ChronoWall reads **GNOME dynamic wallpaper XML files**, evaluates the active timeline event based on the current time of day, and applies the corresponding image to **Cinnamon** (via `gsettings`) or **KDE Plasma** (via `plasma-apply-wallpaperimage`).

Instead of continuously running a GPU rendering loop or holding open a window surface, ChronoWall changes the wallpaper at timeline event boundaries and then **sleeps until the next event transition**.

### Why ChronoWall?

- ⚡ **Lightweight C++17 daemon** — minimal CPU usage and low memory footprint
- 🕐 **Time-aware scheduling** — evaluates 24-hour timeline cycles and event durations
- 🖼️ **GNOME-style XML compatibility** — parses standard `<starttime>`, `<static>`, and `<transition>` tags
- 🧩 **Relative image-path support** — self-contained wallpaper packages
- 🖥️ **Native desktop integration** — first-class support for **Cinnamon (X11)** and **KDE Plasma 6 / 5 (Wayland & X11)**
- 🔋 **Zero persistent sleep overhead** — sleeps via `nanosleep()` between event boundaries
- ⚙️ **systemd user-service integration** — manages backgrounds cleanly within the user's graphical session
- 🚫 **No permanent rendering window** — integrates directly with desktop environment wallpaper engines
- ⌨️ **Simple CLI** — manage, list, inspect, and switch wallpapers easily

---

# 🎨 See the Wallpapers in Action

ChronoWall is the **engine**. Full-resolution wallpaper packages, XML definitions, and animated previews are maintained in the separate companion repository:

👉 **[ChronoWall Wallpapers Repository](https://github.com/Abyyy-s/ChronoWall-Wallpapers)**

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

# 🚀 Installation & Setup

## 1. Prerequisites

### Build Dependencies
ChronoWall requires only standard C++17 build tools and TinyXML2:

- **Debian / Ubuntu / Linux Mint**:
  ```bash
  sudo apt install build-essential cmake libtinyxml2-dev
  ```

- **Fedora**:
  ```bash
  sudo dnf install gcc-c++ cmake tinyxml2-devel
  ```

### Desktop Runtime Prerequisites
ChronoWall interfaces with the desktop's native wallpaper utility at runtime:
- **KDE Plasma (Wayland / X11)**: `plasma-apply-wallpaperimage` (provided by `plasma-workspace`, installed by default on KDE Plasma).
- **Cinnamon (X11)**: `gsettings` (installed by default on Cinnamon).

---

## 2. Get the Wallpaper Collection

ChronoWall itself does not bundle the full wallpaper asset packages. Download or clone the companion collection:

```bash
git clone https://github.com/Abyyy-s/ChronoWall-Wallpapers.git ~/Projects/ChronoWall-Wallpapers
```

*(Alternatively, download the latest archive from [ChronoWall Wallpapers Releases](https://github.com/Abyyy-s/ChronoWall-Wallpapers/releases).)*

A dynamic wallpaper package is organized as follows:
```text
wallpapers/
└── StepbyStep/
    ├── StepbyStep.xml
    └── StepbyStep/
        ├── StepbyStep-1.png
        └── StepbyStep-2.png
```

---

## 3. Clone & Install ChronoWall

From the ChronoWall source directory, run `install.sh` and pass the path to any wallpaper XML from your collection:

```bash
git clone https://github.com/Abyyy-s/ChronoWall.git
cd ChronoWall

./install.sh ~/Projects/ChronoWall-Wallpapers/wallpapers/StepbyStep/StepbyStep.xml
```

### What `install.sh` does:
1. **Detects Desktop**: Automatically identifies KDE Plasma vs Cinnamon and validates runtime tools.
2. **Compiles Release Binary**: Builds `ChronoWall` with CMake in Release mode.
3. **Installs Binary**: Installs executable to `~/.local/bin/chronowall`.
4. **Configures PATH**: Verifies if `~/.local/bin` is in `$PATH` and automatically adds it to your shell configuration (`~/.zshrc`, `~/.bashrc`, or `~/.profile`) if needed.
5. **Deploys Wallpaper Package**: Copies the initial wallpaper package to `~/.config/chronowall/` and saves the collection path.
6. **Configures & Starts Service**: Creates and enables a `systemd --user` service (`chronowall.service`) bound to `graphical-session.target`.

> [!NOTE]
> If `~/.local/bin` was just added to your shell configuration by the installer, reload your shell (`source ~/.zshrc` or `source ~/.bashrc`) or open a new terminal window for the `chronowall` command to be immediately available.

---

# ⌨️ CLI Usage

ChronoWall provides a complete CLI interface to manage wallpapers and background services:

```text
Usage:
  chronowall run <wallpaper.xml>  Run the daemon in the foreground
  chronowall start                Start the installed user service
  chronowall stop                 Stop the installed user service
  chronowall restart              Restart the installed user service
  chronowall status               Show service status
  chronowall list                 List available wallpapers
  chronowall set <name>           Switch to a wallpaper
  chronowall current              Show the current wallpaper
  chronowall --version            Show version
  chronowall --help               Show this help
```

### Common Commands

- **List available wallpapers in your collection**:
  ```bash
  chronowall list
  ```

- **Switch active wallpaper by name**:
  ```bash
  chronowall set Mojave
  ```
  *(Note: Wallpaper names match the package directory name and are case-sensitive.)*

- **Show currently active wallpaper**:
  ```bash
  chronowall current
  ```

- **Service management**:
  ```bash
  chronowall status
  chronowall stop
  chronowall start
  chronowall restart
  ```

- **Run in foreground for debugging/testing**:
  ```bash
  chronowall run ~/Projects/ChronoWall-Wallpapers/wallpapers/MagicLake/MagicLake.xml
  ```

---

# 🛠️ Systemd User Service & Logs

ChronoWall runs as a **systemd user service** (`systemctl --user`), so `sudo` is never required.

- **Check Service Status**:
  ```bash
  chronowall status
  # or directly:
  systemctl --user status chronowall.service
  ```

- **Follow Live Logs**:
  ```bash
  journalctl --user -u chronowall.service -f
  ```

- **Start / Stop / Restart**:
  ```bash
  chronowall stop
  chronowall start
  chronowall restart
  ```

---

# 🧠 How It Works & Architecture

```text
┌──────────────────────────────┐
│ GNOME Dynamic Wallpaper XML  │
└──────────────┬───────────────┘
               ↓
       ┌───────────────┐
       │ WallpaperParser│  (Parses XML, resolves image paths)
       └───────┬───────┘
               ↓
       ┌───────────────┐
       │    Timeline   │  (Calculates 24-hour event timeline)
       └───────┬───────┘
               ↓
       ┌───────────────┐
       │ Wallpaper     │
       │ Scheduler     │  (Identifies active timeline event)
       └───────┬───────┘
               ↓
       ┌───────────────┐
       │ Wallpaper     │
       │ Changer       │  (Desktop-agnostic coordinator)
       └───────┬───────┘
               ↓
       ┌───────────────┐
       │ Wallpaper     │
       │ BackendFactory│  (Deterministic environment detection)
       └───┬───────┬───┘
           │       │
           ▼       ▼
    Cinnamon       KDE Plasma
  (gsettings)    (plasma-apply-wallpaperimage)
```

### Core Architecture Components
- **`WallpaperParser`**: Parses GNOME XML definitions and resolves image paths.
- **`DynamicWallpaper`**: Data model for frames, transitions, and timeline.
- **`WallpaperScheduler`**: Evaluates timeline events based on local time of day.
- **`WallpaperBackend`**: Abstract interface for desktop wallpaper integration.
- **`CinnamonBackend`**: Applies wallpaper via Cinnamon's `gsettings` desktop background schema.
- **`KDEPlasmaBackend`**: Applies wallpaper across all connected displays via KDE's `plasma-apply-wallpaperimage`.
- **`WallpaperBackendFactory`**: Determines active desktop environment or explicit `CHRONOWALL_BACKEND` override.
- **`WallpaperChanger`**: Coordinates wallpaper application with the active backend.

---

# ⚙️ Configuration & Backend Override

ChronoWall automatically detects whether it is running under **KDE Plasma** or **Cinnamon** using session variables (`XDG_CURRENT_DESKTOP`, `XDG_SESSION_DESKTOP`, `DESKTOP_SESSION`).

To explicitly force a specific backend, set `CHRONOWALL_BACKEND`:

```bash
# Force KDE Plasma backend
CHRONOWALL_BACKEND=kde chronowall run /path/to/wallpaper.xml

# Force Cinnamon backend
CHRONOWALL_BACKEND=cinnamon chronowall run /path/to/wallpaper.xml
```

---

# 🧪 Manual Compilation

Build manually using CMake:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

Run directly:
```bash
./build/ChronoWall run /path/to/wallpaper.xml
```

---

# 🗑️ Uninstallation

From the ChronoWall repository:

```bash
./uninstall.sh
```

This stops and disables the systemd user service, removes `~/.local/bin/chronowall`, removes the service unit, and removes configuration at `~/.config/chronowall/`.

---

# 🗺️ Current Scope

| Area | Status |
|---|---|
| Linux (Fedora, Debian, Ubuntu, Mint, Arch, etc.) | ✅ |
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

# 🤝 Contributing

Found a bug, broken XML file, or compatibility issue?
Open an issue or submit a pull request on GitHub.

For wallpaper contributions, visit the companion [ChronoWall Wallpapers](https://github.com/Abyyy-s/ChronoWall-Wallpapers) repository.

---

# 📜 License

ChronoWall is released under the **MIT License**. See [`LICENSE`](LICENSE) for details.

---

<p align="center">
  <strong>🕒 ChronoWall</strong><br>
  Dynamic wallpapers, scheduled beautifully on Linux.
</p>
