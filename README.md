# ChronoWall

> Bring GNOME-style dynamic wallpapers to the Cinnamon desktop.

ChronoWall is a lightweight C++17 daemon that reads GNOME-style dynamic wallpaper XML files and schedules their images on Cinnamon using `gsettings`.

## v1.0.0

ChronoWall v1 focuses on one job: make dynamic wallpaper timelines usable on Cinnamon without creating a permanent rendering window or running a continuous GPU/rendering loop.

ChronoWall parses the XML, determines the active timeline event from local time, applies the appropriate image through Cinnamon's wallpaper API, and then sleeps until the next event boundary.

### Features

- Parse GNOME dynamic wallpaper XML files
- Respect `<starttime>`, `<static>`, and `<transition>` timeline entries
- Correctly select the active event based on local time
- Apply wallpapers through Cinnamon's `gsettings` API
- Long-running daemon with signal-safe shutdown
- systemd user-service integration
- Simple CLI: `start`, `stop`, `restart`, `status`, and `run`
- Release builds through CMake
- Very low idle resource usage
- No SDL, X11, or custom rendering window required by v1

> **Important:** v1 preserves the timing of transition entries but does not perform a true pixel-by-pixel crossfade. The destination image is handed to Cinnamon and ChronoWall waits for the transition duration before advancing to the next event. Live crossfade rendering remains experimental and is kept outside the v1 implementation.

## Requirements

ChronoWall v1 targets **Linux Mint / Cinnamon**.

On Debian/Ubuntu-based systems, install the build dependencies with:

```bash
sudo apt install build-essential cmake libtinyxml2-dev
```

`gsettings` is normally already available on Cinnamon desktops.

## Installation

Clone the repository and run the installer with a dynamic wallpaper XML file:

```bash
git clone https://github.com/Abyyy-s/ChronoWall.git
cd ChronoWall
./install.sh /path/to/wallpaper.xml
```

The installer:

1. Builds a Release binary with CMake.
2. Installs `chronowall` into `~/.local/bin`.
3. Copies the selected XML to `~/.config/chronowall/wallpaper.xml`.
4. Creates a systemd user service.
5. Enables and starts the service.

Make sure `~/.local/bin` is in your `PATH`. If it is not, start a new shell or add it to your shell configuration.

### Example

```bash
./install.sh ~/Linux_Dynamic_Wallpapers/Dynamic_Wallpapers/StepbyStep.xml
```

## CLI

```bash
chronowall --help
chronowall --version
chronowall status
chronowall start
chronowall stop
chronowall restart
```

For debugging, run the daemon directly in the foreground:

```bash
chronowall run /path/to/wallpaper.xml
```

The legacy shorthand is also supported:

```bash
chronowall /path/to/wallpaper.xml
```

## Service and logs

ChronoWall runs as a **systemd user service**, so `sudo` is not required to manage it.

```bash
systemctl --user status chronowall.service
journalctl --user -u chronowall.service -f
```

The service is enabled during installation and is intended to run with the user's graphical session.

## Uninstall

From the repository:

```bash
./uninstall.sh
```

This stops and disables the service and removes:

- `~/.local/bin/chronowall`
- `~/.config/systemd/user/chronowall.service`
- `~/.config/chronowall/`

## How it works

```text
GNOME dynamic wallpaper XML
            |
            v
     WallpaperParser
            |
            v
       Timeline
            |
            v
    WallpaperScheduler
            |
            v
     WallpaperChanger
            |
            v
       gsettings
            |
            v
         Cinnamon
```

The daemon does not continuously render the desktop. It evaluates the current timeline event, changes the wallpaper when the event changes, and sleeps until the next boundary.

For a static event, the corresponding image is applied and the daemon sleeps for the event duration.

For a transition event, the destination image is applied through Cinnamon's normal wallpaper API and the daemon waits for the transition duration. This keeps the XML timeline synchronized while avoiding the CPU/GPU-heavy rendering approach used by the experimental renderer.

## Architecture

The v1 implementation keeps the original parser/model/scheduler architecture:

- `WallpaperParser`
- `DynamicWallpaper`
- `WallpaperFrame`
- `Transition`
- `TimelineEvent`
- `WallpaperScheduler`
- `WallpaperChanger`

The runtime path is intentionally small:

```text
XML
 ↓
Parser
 ↓
Scheduler
 ↓
WallpaperChanger
 ↓
gsettings
 ↓
Cinnamon
```

Experimental SDL3/X11 rendering work is intentionally kept separate from the v1 implementation.

## Development

Build manually with CMake:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

Run the daemon directly:

```bash
./build/ChronoWall run /path/to/wallpaper.xml
```

## Troubleshooting

### Check whether the daemon is running

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

## Current scope

v1 targets Cinnamon on Linux/X11. Other desktop environments, Windows support, and true live crossfade rendering are future work.

## License

MIT License. See [LICENSE](LICENSE) for details.
