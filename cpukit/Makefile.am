ACLOCAL_AMFLAGS = -I aclocal

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/multilib.am

# librtemscpu
_SUBDIRS = . score rtems sapi posix
_SUBDIRS += dev
_SUBDIRS += dtc/libfdt
_SUBDIRS += libcrypt
_SUBDIRS += libcsupport libblock libfs
_SUBDIRS += libdrvmgr
_SUBDIRS += libnetworking librpc
_SUBDIRS += libpci
_SUBDIRS += libi2c
_SUBDIRS += libmisc
_SUBDIRS += libmd
_SUBDIRS += libgnat
_SUBDIRS += libdl
_SUBDIRS += libstdthreads
_SUBDIRS += libdebugger
_SUBDIRS += wrapup

_SUBDIRS += zlib

# applications
_SUBDIRS += ftpd
_SUBDIRS += telnetd
_SUBDIRS += pppd
_SUBDIRS += mghttpd

if LIBNETWORKING
endif

if LIBDL
endif

if LIBDEBUGGER
endif

if !LIBPCI
endif
## should be conditional and only installed for PowerPC, x86, and ARM
## libfs
## POSIX FIFO/pipe
## devfs
if LIBDOSFS
endif

# RFS
# JFFS2
## libblock
## ftpd
if LIBNETWORKING
endif

## bspcmdline
## capture
# tracing headers
## cpuuse
## devnull
## dumpbuf
## rtemsfdt
## monitor
## fb
## mouse
## libqos
## redirector
## shell
if LIBSHELL
endif

## stringto
## testsupport
## i2c
## serdbg
if LIBSERDBG
endif

## stackchk
## libuntar
## fsmount
## Driver manager
if LIBDRVMGR
endif

if LIBGNAT
endif

## HACK: doxygen filter.
EXTRA_DIST = doxy-filter

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
