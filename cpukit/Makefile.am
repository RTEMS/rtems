##
## $Id$
##

ACLOCAL_AMFLAGS = -I aclocal

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/multilib.am

SUBDIRS = . ada score rtems sapi posix itron
SUBDIRS += libcsupport libblock libfs
SUBDIRS += libnetworking librpc
SUBDIRS += libmisc
SUBDIRS += httpd
SUBDIRS += ftpd
SUBDIRS += wrapup

CLEANFILES =
DISTCLEANFILES =

noinst_DATA = preinstall-stamp
preinstall-stamp:
	$(MAKE) $(AM_MAKEFLAGS) preinstall
	touch preinstall-stamp
CLEANFILES += preinstall-stamp

include_HEADERS =

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS =

include_rtems_rtemsdir = $(includedir)/rtems/rtems
include_rtems_rtems_HEADERS =

if DEPRECATED
include_HEADERS += backward/asm.h backward/chain.h backward/clockdrv.h \
    backward/confdefs.h backward/console.h backward/imfs.h backward/iosupp.h \
    backward/ringbuf.h backward/rtc.h backward/spurious.h \
    backward/timerdrv.h backward/vmeintr.h

if HAS_NETWORKING
include_HEADERS += backward/ftpd.h
endif

include_motoroladir = $(includedir)/motorola
include_motorola_HEADERS = backward/motorola/mc68681.h \
    backward/motorola/mc68230.h

include_zilogdir = $(includedir)/zilog
include_zilog_HEADERS = backward/zilog/z8536.h backward/zilog/z8036.h \
    backward/zilog/z8030.h

if !UNIX
include_HEADERS += backward/dosfs.h
endif

if HAS_ITRON
include_HEADERS += backward/itron.h
endif
endif

if NEWLIB
include_sysdir = $(includedir)/sys
include_sys_HEADERS =

if RTEMS_PROVIDES_STDINT_H
include_HEADERS += libcsupport/include/stdint.h
endif
if RTEMS_PROVIDES_INTTYPES_H
include_HEADERS += libcsupport/include/inttypes.h
endif
include_HEADERS += libcsupport/include/tar.h

include_sys_HEADERS += libcsupport/include/sys/ioccom.h \
    libcsupport/include/sys/filio.h libcsupport/include/sys/ioctl.h \
    libcsupport/include/sys/sockio.h libcsupport/include/sys/ttycom.h \
    libcsupport/include/sys/termios.h libcsupport/include/sys/utsname.h \
    libcsupport/include/sys/cdefs.h
endif

include_rtems_HEADERS += include/rtems/bspIo.h include/rtems/userenv.h \
    include/rtems/fs.h include/rtems/pci.h include/rtems/stdint.h \
    include/rtems/concat.h include/rtems/tar.h

## libfs
include_rtems_HEADERS += libfs/src/imfs/imfs.h

if !UNIX
include_rtems_HEADERS += libfs/src/dosfs/dosfs.h
endif

## libblock
if !UNIX
include_rtems_HEADERS += libblock/include/rtems/bdbuf.h \
    libblock/include/rtems/blkdev.h libblock/include/rtems/diskdevs.h \
    libblock/include/rtems/ramdisk.h \
    libblock/include/rtems/ide_part_table.h
endif

## ftpd
if HAS_NETWORKING
include_rtems_HEADERS += ftpd/ftpd.h
endif

## capture
include_rtems_HEADERS += libmisc/capture/capture.h \
    libmisc/capture/capture-cli.h

## cpuuse
include_rtems_HEADERS += libmisc/cpuuse/cpuuse.h

## devnull
include_rtems_HEADERS += libmisc/devnull/devnull.h

## dumpbuf
include_rtems_HEADERS += libmisc/dumpbuf/dumpbuf.h

## monitor

include_rtems_HEADERS += libmisc/monitor/monitor.h

## mw-fb
include_rtems_HEADERS += libmisc/mw-fb/mw_fb.h libmisc/mw-fb/mw_uid.h

## shell
if LIBSHELL
include_rtems_HEADERS += libmisc/shell/shell.h
endif

## rtmonuse
include_rtems_HEADERS += libmisc/rtmonuse/rtmonuse.h

## serdbg
if LIBSERDBG
include_rtems_HEADERS += libmisc/serdbg/serdbgcnf.h libmisc/serdbg/serdbg.h \
    libmisc/serdbg/termios_printk_cnf.h libmisc/serdbg/termios_printk.h
endif

## stackchk
include_rtems_HEADERS += libmisc/stackchk/stackchk.h

## libuntar
include_rtems_HEADERS += libmisc/untar/untar.h

## fsmount
include_rtems_HEADERS += libmisc/fsmount/fsmount.h

all-local: $(PREINSTALL_FILES)

include $(srcdir)/preinstall.am

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
