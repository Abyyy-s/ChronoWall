# ChronoWall Architecture

ChronoWall is structured around modular, desktop-independent core components and pluggable desktop wallpaper backends.

## Core Concepts

- **Wallpaper Image**: Individual static image file referenced by the XML definition.
- **WallpaperFrame**: Representation of an image asset and its active display duration.
- **Transition**: Dynamic transition event definition between a source and destination image.
- **TimelineEvent**: 24-hour timeline event scheduler unit (static frame or transition window).
- **DynamicWallpaper**: Complete dynamic wallpaper data model parsed from GNOME-style XML.
- **WallpaperScheduler**: Evaluates elapsed seconds against the timeline to identify the active event.
- **WallpaperBackend**: Abstract interface for applying wallpapers to the user's desktop environment.
- **WallpaperBackendFactory**: Deterministic factory responsible for runtime desktop detection and backend instantiation.
- **WallpaperChanger**: High-level coordinator that delegates wallpaper updates to the active backend.

## Architecture Flow

```text
GNOME Dynamic Wallpaper XML
            │
            ▼
     WallpaperParser
            │
            ▼
     DynamicWallpaper
            │
            ▼
    WallpaperScheduler
            │
            ▼
     WallpaperChanger
            │
            ▼
  WallpaperBackendFactory
      │             │
      ▼             ▼
CinnamonBackend  KDEPlasmaBackend
 (gsettings)   (plasma-apply-wallpaperimage)
```
