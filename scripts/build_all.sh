#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
make cases OPT=O2
make CASE=2 opts
printf '\nGenerated binaries:\n'
ls -lh output/*.bin
