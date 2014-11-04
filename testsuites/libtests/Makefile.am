ACLOCAL_AMFLAGS = -I ../aclocal

_SUBDIRS = POSIX
_SUBDIRS += newlib01
_SUBDIRS += block17
_SUBDIRS += exit02
_SUBDIRS += exit01
_SUBDIRS += utf8proc01
_SUBDIRS += md501
_SUBDIRS += sparsedisk01
_SUBDIRS += block16
_SUBDIRS += block15
_SUBDIRS += block14
_SUBDIRS += block13
_SUBDIRS += rbheap01
_SUBDIRS += flashdisk01
_SUBDIRS += capture01

_SUBDIRS += bspcmdline01 cpuuse devfs01 devfs02 devfs03 devfs04 \
    deviceio01 devnullfatal01 dumpbuf01 gxx01 top\
    malloctest malloc02 malloc03 malloc04 malloc05 heapwalk \
    putenvtest monitor monitor02 rtmonuse stackchk stackchk01 \
    termios termios01 termios02 termios03 termios04 termios05 \
    termios06 termios07 termios08 \
    rtems++ tztest block01 block02 block03 block04 block05 block06 block07 \
    block08 block09 block10 block11 block12 stringto01 \
    tar01 tar02 tar03 \
    math mathf mathl complex \
    mouse01 uid01

if NETTESTS
if HAS_POSIX
_SUBDIRS += mghttpd01
endif
_SUBDIRS += ftp01
_SUBDIRS += syscall01
endif

if DLTESTS
_SUBDIRS += dl01 dl02
endif

include $(top_srcdir)/../automake/test-subdirs.am
include $(top_srcdir)/../automake/local.am
