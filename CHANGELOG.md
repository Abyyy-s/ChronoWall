# Changelog

## [2.0.0] - 2026-08-23

### Added

- **Multi-Desktop Support**: Native support for **KDE Plasma 6 / 5** (Wayland & X11) alongside **Cinnamon (X11)**.
- **Desktop Wallpaper Backend Architecture**: Pluggable `WallpaperBackend` abstraction cleanly separating desktop integrations from core scheduling and parsing.
- **KDE Plasma Backend**: Integrates with KDE's native `plasma-apply-wallpaperimage` utility to apply wallpapers across all connected monitors/displays.
- **Cinnamon Backend**: Retains native `gsettings` integration for Linux Mint / Cinnamon desktops.
- **Deterministic Desktop Factory**: `WallpaperBackendFactory` provides automatic desktop environment detection (`XDG_CURRENT_DESKTOP`, `XDG_SESSION_DESKTOP`, `DESKTOP_SESSION`) and explicit backend overrides via `CHRONOWALL_BACKEND=<kde|cinnamon>`.
- **Modernized Systemd User Service**: Clean user service integration bound to `graphical-session.target` relying on session D-Bus without X11-only display dependencies.
- **Shell PATH Auto-Configuration**: Installer automatically verifies and configures `~/.local/bin` in user shell profiles (`~/.zshrc`, `~/.bashrc`, `~/.profile`).
- **Comprehensive Documentation**: Detailed architecture guide, multi-distro dependency guides (Debian/Ubuntu, Fedora), and companion wallpaper repository workflow.

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
