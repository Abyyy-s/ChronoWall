# ChronoWall

> Bring GNOME Dynamic Wallpapers to the Cinnamon Desktop.

ChronoWall is a lightweight C++17 daemon that reads GNOME-style dynamic wallpaper XML files and schedules their images on Cinnamon using `gsettings`.

## v1.0.0

ChronoWall v1 focuses on one job: make GNOME dynamic wallpaper timelines usable on Cinnamon without creating a permanent rendering window or running a GPU loop in the background.

The XML is parsed by ChronoWall, the active timeline event is calculated from local time, and Cinnamon remains responsible for displaying the wallpaper.

### Features

- Parse GNOME dynamic wallpaper XML files
- Respect `<starttime>`, `<static>`, and `<transition>` timeline entries
- Correctly select the active event at any time of day
- Apply wallpapers through Cinnamon's `gsettings` API
- Long-running daemon with signal-safe shutdown
- Foreground and systemd user-service operation
- Simple CLI: `start`, `stop`, `restart`, `status`, and `run`
- Release builds through CMake
- No SDL, X11, or custom rendering loop required by v1

## Requirements

ChronoWall v1 targets **Linux Mint / Cinnamon**.

Install these packages on Debian/Ubuntu-based systems:

```bash
sudo apt install build-essential cmake libtinyxml2-dev
```

`gsettings` is normally already available on Cinnamon desktops.

## Install

Clone the repository and run the installer with a dynamic wallpaper XML file:

```bash
git clone https://github.com/Abyyy-s/ChronoWall.git
cd ChronoWall
git checkout renderer-daemon
./install.sh /path/to/wallpaper.xml
```

The installer:

1. Builds a Release binary.
2. Installs `chronowall` into `~/.local/bin`.
3. Copies the selected XML into `~/.config/chronowall/wallpaper.xml`.
4. Creates a systemd user service.
5. Enables and starts ChronoWall automatically.

Make sure `~/.local/bin` is in your `PATH`. If it is not, start a new shell or add it to your shell configuration.

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

The legacy shorthand also works:

```bash
chronowall /path/to/wallpaper.xml
```

## Service and logs

ChronoWall runs as a user service, so it does not require `sudo` to start or stop.

```bash
systemctl --user status chronowall.service
journalctl --user -u chronowall.service -f
```

The service starts automatically with the user's graphical session.

## Uninstall

From the repository:

```bash
./uninstall.sh
```

This stops and disables the service and removes the installed binary, service file, and ChronoWall configuration directory.

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

ChronoWall does not continuously render the desktop. During static periods it applies the required wallpaper and sleeps until the next timeline boundary. During transition events v1 selects the destination image through the normal Cinnamon wallpaper API and still waits for the transition duration so the XML timeline remains synchronized.

## Architecture

The original parser/model/scheduler architecture remains intact:

- `WallpaperParser`
- `DynamicWallpaper`
- `WallpaperFrame`
- `Transition`
- `TimelineEvent`
- `WallpaperScheduler`
- `WallpaperChanger`

Experimental SDL3/GPU rendering work is intentionally kept on a separate branch and is not part of v1.

## Development

Build manually:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

Run:

```bash
./build/ChronoWall run /path/to/wallpaper.xml
```

## Current scope

v1 targets Cinnamon on Linux. Other desktop environments and custom rendering backends are future work.

## License

MIT License. See [LICENSE](LICENSE).
