##
## $Id$
##

ACLOCAL_AMFLAGS = -I aclocal

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/multilib.am

# librtemscpu
SUBDIRS = . score rtems sapi posix itron
SUBDIRS += libcsupport libblock libfs
SUBDIRS += libnetworking librpc
SUBDIRS += libi2c
SUBDIRS += libmisc
SUBDIRS += wrapup

SUBDIRS += zlib

# applications
SUBDIRS += httpd
SUBDIRS += ftpd
SUBDIRS += telnetd
SUBDIRS += pppd

noinst_DATA = preinstall-stamp
preinstall-stamp:
	$(MAKE) $(AM_MAKEFLAGS) preinstall
	touch preinstall-stamp
CLEANFILES = preinstall-stamp

include_HEADERS =

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS =

include_rtems_rtemsdir = $(includedir)/rtems/rtems
include_rtems_rtems_HEADERS =

include_rtems_bsd_sysdir = $(include_rtemsdir)/bsd/sys
include_rtems_bsd_sys_HEADERS = include/rtems/bsd/sys/queue.h
include_rtems_bsd_sys_HEADERS += include/rtems/bsd/sys/cdefs.h

if NEWLIB
include_sysdir = $(includedir)/sys
include_sys_HEADERS =

include_HEADERS += libnetworking/memory.h

include_HEADERS += libcsupport/include/tar.h

include_sys_HEADERS += libcsupport/include/sys/ioccom.h \
    libcsupport/include/sys/filio.h libcsupport/include/sys/ioctl.h \
    libcsupport/include/sys/sockio.h libcsupport/include/sys/ttycom.h \
    libcsupport/include/sys/termios.h libcsupport/include/sys/utsname.h \
    libcsupport/include/sys/cdefs.h

if LIBNETWORKING
include_rtems_bsdnetdir = $(includedir)/rtems/bsdnet
include_rtems_bsdnet_HEADERS = libnetworking/rtems/bsdnet/servers.h
include_rtems_bsdnet_HEADERS += libnetworking/rtems/bsdnet/_types.h
endif
endif

include_rtems_HEADERS += include/rtems/bspIo.h include/rtems/userenv.h \
    include/rtems/fs.h include/rtems/pci.h include/rtems/stdint.h \
    include/rtems/concat.h include/rtems/tar.h

include_rtems_HEADERS += include/rtems/endian.h

## should be conditional and only installed for PowerPC, x86, and ARM
include_rtems_HEADERS += include/rtems/irq.h

## libfs
include_rtems_HEADERS += libfs/src/imfs/imfs.h

if LIBDOSFS
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
if LIBNETWORKING
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

## i2c
include_rtems_HEADERS += libi2c/libi2c.h

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

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
