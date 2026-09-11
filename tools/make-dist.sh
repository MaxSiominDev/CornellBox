#!/bin/sh
set -eu

ROOT=$(cd "$(dirname "$0")/.." && pwd)
NAME=cornellbox-semin
STAGE="$ROOT/dist/$NAME"

cd "$ROOT"
for binary in cornellbox cornellbox.exe; do
    if [ ! -f "$binary" ]; then
        echo "$binary is missing; build it first" >&2
        exit 1
    fi
done

rm -rf "$ROOT/dist"
mkdir -p "$STAGE/screenshots"

./cornellbox --size 800 --press h --screenshot "$STAGE/screenshots/01-canonical.bmp"
./cornellbox --size 800 --press h12345 --screenshot "$STAGE/screenshots/02-lighting-only.bmp"
./cornellbox --size 800 --press h3 --screenshot "$STAGE/screenshots/03-no-shadows.bmp"
./cornellbox --size 800 --press hddddddwww- --screenshot "$STAGE/screenshots/04-orbit.bmp"
./cornellbox --size 800 --press hssss++++++ --screenshot "$STAGE/screenshots/05-close-up.bmp"
for bmp in "$STAGE"/screenshots/*.bmp; do
    sips -s format png "$bmp" --out "${bmp%.bmp}.png" > /dev/null
    rm "$bmp"
done

cp -R src tests tools Makefile .gitignore readme.txt cornellbox.exe "$STAGE/"
(cd "$ROOT/dist" && zip -qr "$NAME.zip" "$NAME")

echo "packed dist/$NAME.zip"
