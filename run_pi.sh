#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

exec "$script_dir/build/pi_receiver" \
  --bind 0.0.0.0 \
  --port 5000 \
  --width 1920 \
  --height 1080 \
  --fps 60 \
  --bitrate 12000
