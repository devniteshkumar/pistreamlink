#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

if [[ -z "${PIPEWIRE_TARGET:-}" ]]; then
  if ! command -v pw-cli >/dev/null 2>&1; then
    printf 'run_pc.sh: pw-cli is required to discover a PipeWire video source\n' >&2
    exit 1
  fi
  PIPEWIRE_TARGET="$(
    pw-cli ls Node | awk '
      /^[[:space:]]*id / {
        node_id = $2
        sub(/,$/, "", node_id)
      }
      /media.class = "Video\/Source"/ {
        print node_id
        exit
      }
    '
  )"
fi

if [[ -z "$PIPEWIRE_TARGET" ]]; then
  printf 'run_pc.sh: no PipeWire video source found; select one with PIPEWIRE_TARGET\n' >&2
  exit 1
fi

PIPEWIRE_FORMAT="${PIPEWIRE_FORMAT:-raw}"
if [[ "$PIPEWIRE_FORMAT" == "raw" ]] &&
   pw-cli info "$PIPEWIRE_TARGET" 2>/dev/null | grep -q 'media.role = "Camera"'; then
  PIPEWIRE_FORMAT=mjpeg
fi

exec "$script_dir/build/pc_sender" \
  --host 10.171.14.142 \
  --port 5000 \
  --source pipewire \
  --pipewire-target "$PIPEWIRE_TARGET" \
  --pipewire-format "$PIPEWIRE_FORMAT" \
  --width 1920 \
  --height 1080 \
  --fps 60 \
  --bitrate 12000
