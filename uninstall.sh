#!/usr/bin/env bash
set -euo pipefail

systemctl --user disable --now chronowall.service 2>/dev/null || true
systemctl --user daemon-reload 2>/dev/null || true

rm -f "${HOME}/.local/bin/chronowall"
rm -f "${HOME}/.config/systemd/user/chronowall.service"
rm -rf "${HOME}/.config/chronowall"

systemctl --user daemon-reload 2>/dev/null || true

echo "ChronoWall has been uninstalled."
