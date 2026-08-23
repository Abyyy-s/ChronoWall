#!/usr/bin/env bash
set -euo pipefail

PREFIX="${HOME}/.local"
CONFIG_DIR="${HOME}/.config/chronowall"
SERVICE_DIR="${HOME}/.config/systemd/user"
BINARY="${PREFIX}/bin/chronowall"
CONFIG_XML="${CONFIG_DIR}/wallpaper.xml"
SERVICE="${SERVICE_DIR}/chronowall.service"

XML_PATH="${1:-}"

if [[ -z "${XML_PATH}" ]]; then
    echo "Usage: bash install.sh <wallpaper.xml>"
    echo "Example: bash install.sh ~/Linux_Dynamic_Wallpapers/Dynamic_Wallpapers/StepbyStep.xml"
    exit 1
fi

if [[ ! -f "${XML_PATH}" ]]; then
    echo "Error: XML file not found: ${XML_PATH}" >&2
    exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
    echo "Error: cmake is required." >&2
    exit 1
fi

DESKTOP="${XDG_CURRENT_DESKTOP:-${XDG_SESSION_DESKTOP:-${DESKTOP_SESSION:-}}}"
DESKTOP_LOWER="$(echo "${DESKTOP}" | tr '[:upper:]' '[:lower:]')"

if [[ "${DESKTOP_LOWER}" =~ kde|plasma ]]; then
    echo "Detected desktop: KDE Plasma"
    if ! command -v plasma-apply-wallpaperimage >/dev/null 2>&1; then
        echo "Error: plasma-apply-wallpaperimage is required (plasma-workspace)." >&2
        exit 1
    fi
elif [[ "${DESKTOP_LOWER}" =~ cinnamon|x-cinnamon ]]; then
    echo "Detected desktop: Cinnamon"
    if ! command -v gsettings >/dev/null 2>&1; then
        echo "Error: gsettings is required (Cinnamon desktop)." >&2
        exit 1
    fi
else
    echo "Warning: Desktop environment '${DESKTOP}' not recognized automatically."
    echo "Checking available wallpaper backend tools..."
    if command -v plasma-apply-wallpaperimage >/dev/null 2>&1; then
        echo "Found plasma-apply-wallpaperimage (KDE Plasma backend)."
    elif command -v gsettings >/dev/null 2>&1; then
        echo "Found gsettings (Cinnamon backend)."
    else
        echo "Error: Neither plasma-apply-wallpaperimage (KDE) nor gsettings (Cinnamon) was found." >&2
        exit 1
    fi
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build-release"

cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${BUILD_DIR}" --config Release -j"$(nproc)"

mkdir -p "${PREFIX}/bin" "${CONFIG_DIR}" "${SERVICE_DIR}"

install -m 0755 "${BUILD_DIR}/ChronoWall" "${BINARY}"

# Ensure ~/.local/bin is in PATH for shell sessions
if [[ ":${PATH}:" != *":${HOME}/.local/bin:"* ]]; then
    SHELL_NAME="$(basename "${SHELL:-bash}")"
    SHELL_RC=""
    if [[ "${SHELL_NAME}" == "zsh" && -f "${HOME}/.zshrc" ]]; then
        SHELL_RC="${HOME}/.zshrc"
    elif [[ -f "${HOME}/.bashrc" ]]; then
        SHELL_RC="${HOME}/.bashrc"
    elif [[ -f "${HOME}/.profile" ]]; then
        SHELL_RC="${HOME}/.profile"
    fi

    if [[ -n "${SHELL_RC}" ]]; then
        if ! grep -Eq '^[[:space:]]*export[[:space:]]+PATH=.*\.local/bin' "${SHELL_RC}" 2>/dev/null; then
            printf '\n# Added by ChronoWall\nexport PATH="%s/.local/bin:${PATH}"\n' "${HOME}" >> "${SHELL_RC}"
            echo "Added ~/.local/bin to PATH in ${SHELL_RC}."
        fi
    fi
    export PATH="${HOME}/.local/bin:${PATH}"
fi

# Copy the complete wallpaper package so relative image paths work.
XML_DIR="$(cd "$(dirname "${XML_PATH}")" && pwd)"
cp -a "${XML_DIR}/." "${CONFIG_DIR}/"

# Remember the wallpaper collection for the CLI selector.
WALLPAPER_LIBRARY="$(dirname "${XML_DIR}")"
printf '%s\n' "${WALLPAPER_LIBRARY}" > "${CONFIG_DIR}/wallpaper-library"
printf '%s\n' "$(basename "${XML_DIR}")" > "${CONFIG_DIR}/current-wallpaper"

# The systemd service always uses this stable XML path.
install -m 0644 "${XML_PATH}" "${CONFIG_XML}"
cat > "${SERVICE}" <<EOF
[Unit]
Description=ChronoWall dynamic wallpaper daemon
PartOf=graphical-session.target
After=graphical-session.target

[Service]
Type=simple
ExecStart=${BINARY} run ${CONFIG_XML}
Restart=on-failure
RestartSec=2

[Install]
WantedBy=graphical-session.target default.target
EOF

systemctl --user daemon-reload
systemctl --user enable --now chronowall.service

echo
echo "ChronoWall installed successfully."
echo "Version: 1.0.0"
echo "Wallpaper: ${CONFIG_XML}"
echo
echo "Commands:"
echo "  chronowall list"
echo "  chronowall set <wallpaper>"
echo "  chronowall current"
echo "  chronowall start"
echo "  chronowall stop"
echo "  chronowall restart"
echo "  chronowall status"
echo
echo "Logs:"
echo "  journalctl --user -u chronowall.service -f"
