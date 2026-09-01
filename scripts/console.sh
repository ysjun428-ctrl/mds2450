#!/usr/bin/env bash
set -euo pipefail
DEV="${1:-/dev/ttyUSB0}"
BAUD="${2:-115200}"
echo "Opening $DEV at $BAUD baud. picocom exit: Ctrl-A Ctrl-X"
exec picocom -b "$BAUD" "$DEV"
