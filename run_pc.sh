#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

exec "$script_dir/build/pc_sender" \
  --host 10.171.14.137 \
  --port 5000 \
  --source pipewire \
  --pipewire-target 67 \
  --width 1920 \
  --height 1080 \
  --fps 60 \
  --bitrate 12000
