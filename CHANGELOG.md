# Changelog

## [Unreleased]

### Added

- Native KDE Plasma 6 / 5 support (Wayland & X11) via `plasma-apply-wallpaperimage`.
- Pluggable desktop wallpaper backend architecture (`WallpaperBackend`, `CinnamonBackend`, `KDEPlasmaBackend`).
- Deterministic desktop detection and backend factory (`WallpaperBackendFactory`).
- Explicit backend override support via `CHRONOWALL_BACKEND=<kde|cinnamon>`.
- Modernized systemd user service with Wayland / D-Bus session support.
- Multi-monitor wallpaper synchronization support under KDE Plasma.

## [1.0.0] - 2026-08-09

### Added

- GNOME dynamic wallpaper XML parsing for Cinnamon.
- Time-aware static and transition event scheduling.
- Lightweight long-running daemon.
- `chronowall run`, `start`, `stop`, `restart`, and `status` commands.
- `--version` and `--help` CLI options.
- systemd user-service installation and automatic startup.
- One-command installer and uninstall script.
- Release-build workflow documented with CMake.
- Foreground mode for debugging.
- Signal-safe shutdown on SIGINT and SIGTERM.

### Design

- v1 uses Cinnamon's `gsettings` wallpaper API instead of a custom SDL/X11 rendering surface.
- Experimental GPU rendering remains isolated from the v1 daemon branch.

### Scope

- Linux Mint / Cinnamon only.
