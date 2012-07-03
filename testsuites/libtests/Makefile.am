ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = POSIX
SUBDIRS += block15
SUBDIRS += block14
SUBDIRS += block13
SUBDIRS += rbheap01
SUBDIRS += flashdisk01

SUBDIRS += bspcmdline01 cpuuse devfs01 devfs02 devfs03 devfs04 \
    deviceio01 devnullfatal01 dumpbuf01 gxx01 \
    malloctest malloc02 malloc03 malloc04 malloc05 heapwalk \
    putenvtest monitor monitor02 rtmonuse stackchk stackchk01 \
    termios termios01 termios02 termios03 termios04 termios05 \
    termios06 termios07 termios08 \
    rtems++ tztest block01 block02 block03 block04 block05 block06 block07 \
    block08 block09 block10 block11 block12 stringto01 \
    tar01 tar02 tar03 \
    math mathf mathl complex \
    mouse01

if NETTESTS
SUBDIRS += mghttpd01
SUBDIRS += ftp01
SUBDIRS += syscall01
endif

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
