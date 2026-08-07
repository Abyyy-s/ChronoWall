# ChronoWall

> Bring GNOME Dynamic Wallpapers to the Cinnamon Desktop.

ChronoWall is an open-source desktop utility written in Modern C++ that enables GNOME Dynamic XML wallpapers to work seamlessly on the Cinnamon desktop environment.

---

## �� 🌟 Features Implemented

- � ✅ **Parse GNOME Dynamic Wallpaper XML files** - Fully compliant with the GNOME background XML format
- � ✅ **Static Event Handling** - Correctly selects and applies static wallpaper images based on time-of-day
- � ✅ **Transition Event Detection** - Accurately identifies transition periods in dynamic wallpapers (rendering pending)
- � ✅ **Cinnamon Desktop Integration** - Uses GNOME Settings via `gsettings` to update the desktop background
- � ✅ **Modern C++17** - Utilizes contemporary C++ standards for safety and performance
- � ✅ **Lightweight & Native** - Minimal dependencies, standalone executable
- � ✅ **Open Source** - MIT licensed

---

## �� 🔧 How to Build

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+
- TinyXML2 library

### Build Steps
```bash
# Clone the repository
git clone https://github.com/yourusername/ChronoWall.git
cd ChronoWall

# Create and enter build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# The executable will be created as './ChronoWall'
```

---

## � ▶��️ How to Run

```bash
# Run with a GNOME dynamic wallpaper XML file
./ChronoWall /path/to/wallpaper.xml

# Example with system wallpaper
./ChronoWall /usr/share/backgrounds/Dynamic_Wallpapers/Aura.xml
```

### Expected Output
- Success: `Wallpaper changed successfully!`
- Error: Informative messages via stderr (missing XML elements, file not found, etc.)
- Usage: `Usage: ./ChronoWall <wallpaper.xml>` (when arguments are incorrect)

---

## �� ⚠��️ Current Limitations

- �� 🔄 **Transitions Detected But Not Rendered** - The application correctly identifies transition periods in dynamic wallpapers but currently only displays the static image at the start of the transition. Full crossfade/overlay transition rendering is planned for future releases.
- �� ⏰ **Timezone Handling** - Uses local system time without explicit timezone conversion (matches GNOME's behavior)
- �� 🖼��️ **Image Format Support** - Relies on the underlying system's image loader (typically supports JPEG, PNG, SVG, etc.)
- �� 🐧 **Desktop Environment** - Currently targets only Cinnamon (GNOME backend would require different settings API)

---

## �� 📈 Vision

ChronoWall aims to provide a lightweight, native, and extensible wallpaper engine for Linux users who want dynamic wallpapers without switching desktop environments.

---

## �� 🎯 Goals

- [x] Parse GNOME Dynamic Wallpaper XML files
- [x] Support Cinnamon Desktop
- [ ] Lightweight (<5MB RAM, minimal CPU)
- [x] Native Linux application
- [x] Modern C++17
- [x] Open Source
- [ ] Add transition rendering (crossfade/overlay)
- [ ] Add system tray indicator
- [ ] Support additional desktop environments (GNOME, KDE Plasma)

---

## �� 🛠��️ Development

ChronoWall is developed with:
- **Modern C++** - Leveraging C++17 features for safety and clarity
- **Clean Architecture** - Separation of concerns: parsing, scheduling, and desktop integration
- **Minimal Dependencies** - Only requires TinyXML2 for XML parsing
- **Thorough Testing** - Verified with multiple GNOME dynamic wallpaper XML files

---

## �� 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## �� 🙏 Acknowledgments

- GNOME Dynamic Wallpaper specification
- TinyXML2 library for XML parsing
- Cinnamon Desktop team for the GSettings API