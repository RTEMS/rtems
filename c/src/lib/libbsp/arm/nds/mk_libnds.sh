#!/bin/sh

[ -d "$1" ] || ( echo "usage: $0 <srcdir>"; exit 1 )

cd "$1"

echo "Symlinking assembly files..."
ln -sf icache.s libnds/source/arm9/icache.S
ln -sf dcache.s libnds/source/arm9/dcache.S
ln -sf exceptionHandler.s libnds/source/arm9/exceptionHandler.S
ln -sf biosCalls.s libnds/source/common/biosCalls.S
ln -sf interruptDispatcher.s libnds/source/common/interruptDispatcher.S

echo "Creating font header..."
cat > libnds/include/default_font_bin.h <<EOF
#ifndef default_font_bin_h_
#define default_font_bin_h_

extern void *default_font_bin;
static void *default_font_bin_ptr = &default_font_bin;

#define default_font_bin default_font_bin_ptr

#endif
EOF

echo "Removing socket.h..."
rm -f dswifi/include/sys/socket.h

echo "Patching libnds..."
patch -p0 < patch.libnds

echo "Done."

cd -
