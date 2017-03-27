ACLOCAL_AMFLAGS = -I aclocal

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/multilib.am

# librtemscpu
SUBDIRS = . score rtems sapi posix
SUBDIRS += dev
SUBDIRS += dtc/libfdt
SUBDIRS += libcrypt
SUBDIRS += libcsupport libblock libfs
SUBDIRS += libdrvmgr
SUBDIRS += libnetworking librpc
SUBDIRS += libpci
SUBDIRS += libi2c
SUBDIRS += libmisc
SUBDIRS += libmd
SUBDIRS += libgnat
SUBDIRS += libdl
SUBDIRS += libstdthreads
SUBDIRS += libdebugger
SUBDIRS += wrapup

SUBDIRS += zlib

# applications
SUBDIRS += ftpd
SUBDIRS += telnetd
SUBDIRS += pppd
SUBDIRS += mghttpd

noinst_DATA = preinstall-stamp
preinstall-stamp:
	$(MAKE) $(AM_MAKEFLAGS) preinstall
	touch preinstall-stamp
CLEANFILES = preinstall-stamp

include_HEADERS =
include_HEADERS += include/endian.h

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS =

include_rtems_rtemsdir = $(includedir)/rtems/rtems
include_rtems_rtems_HEADERS =

include_uuiddir = $(includedir)/uuid
include_uuid_HEADERS = libmisc/uuid/uuid.h

include_utf8procdir = $(includedir)/utf8proc
include_utf8proc_HEADERS = libmisc/utf8proc/utf8proc.h

include_xzdir = $(includedir)
include_xz_HEADERS = libmisc/xz/xz.h

include_sysdir = $(includedir)/sys
include_sys_HEADERS =

include_HEADERS += include/crypt.h
include_HEADERS += include/memory.h

include_sys_HEADERS += libcsupport/include/sys/event.h
include_sys_HEADERS += libcsupport/include/sys/filio.h
include_sys_HEADERS += libcsupport/include/sys/ioccom.h
include_sys_HEADERS += libcsupport/include/sys/ioctl.h
include_sys_HEADERS += libcsupport/include/sys/poll.h
include_sys_HEADERS += libcsupport/include/sys/sockio.h
include_sys_HEADERS += libcsupport/include/sys/statvfs.h
include_sys_HEADERS += libcsupport/include/sys/_termios.h
include_sys_HEADERS += libcsupport/include/sys/termios.h
include_sys_HEADERS += libcsupport/include/sys/ttycom.h
include_sys_HEADERS += libcsupport/include/sys/ttydefaults.h
include_sys_HEADERS += libcsupport/include/sys/utsname.h

include_sys_HEADERS += include/sys/endian.h
include_sys_HEADERS += include/sys/uio.h
include_sys_HEADERS += include/sys/_iovec.h
include_sys_HEADERS += include/sys/priority.h

if LIBNETWORKING
include_rtems_bsdnetdir = $(includedir)/rtems/bsdnet
include_rtems_bsdnet_HEADERS = libnetworking/rtems/bsdnet/servers.h
include_rtems_bsdnet_HEADERS += libnetworking/rtems/bsdnet/_types.h
endif

if LIBDL
include_HEADERS += libdl/dlfcn.h
include_HEADERS += libdl/include/link.h
include_HEADERS += libdl/include/link_elf.h
include_sys_HEADERS += libdl/include/sys/cdefs_elf.h
include_sys_HEADERS += libdl/include/sys/exec_elf.h
include_arch_machinedir = $(includedir)/machine
include_arch_machine_HEADERS =
include_arch_machine_HEADERS += libdl/include/arch/@RTEMS_CPU@/machine/elf_machdep.h
include_rtems_rtldir = $(includedir)/rtems/rtl
include_rtems_rtl_HEADERS =
include_rtems_rtl_HEADERS += libdl/dlfcn-shell.h
include_rtems_rtl_HEADERS += libdl/rtl.h libdl/rtl-allocator.h libdl/rtl-obj-fwd.h
include_rtems_rtl_HEADERS += libdl/rtl-fwd.h libdl/rtl-obj.h libdl/rtl-obj-cache.h
include_rtems_rtl_HEADERS += libdl/rtl-obj-comp.h libdl/rtl-unresolved.h
include_rtems_rtl_HEADERS += libdl/rtl-indirect-ptr.h libdl/rtl-sym.h libdl/rtl-trace.h
include_rtems_rtl_HEADERS += libdl/rap.h libdl/rap-shell.h
endif

if LIBDEBUGGER
include_rtems_HEADERS += libdebugger/rtems-debugger.h
include_rtems_debuggerdir = $(includedir)/rtems/debugger
include_rtems_debugger_HEADERS  =
include_rtems_debugger_HEADERS += libdebugger/rtems-debugger-server.h
include_rtems_debugger_HEADERS +=  libdebugger/rtems-debugger-remote.h
endif

include_rtems_HEADERS += include/rtems/bspIo.h
include_rtems_HEADERS += include/rtems/print.h
include_rtems_HEADERS += include/rtems/printer.h
include_rtems_HEADERS += include/rtems/userenv.h
include_rtems_HEADERS += include/rtems/fs.h
if !LIBPCI
include_rtems_HEADERS += include/rtems/pci.h
endif
include_rtems_HEADERS += include/rtems/concat.h
include_rtems_HEADERS += include/rtems/status-checks.h

include_rtems_HEADERS += include/rtems/endian.h

include_rtems_HEADERS += include/rtems/btimer.h
include_rtems_HEADERS += include/rtems/console.h
include_rtems_HEADERS += include/rtems/clockdrv.h
include_rtems_HEADERS += include/rtems/framebuffer.h
include_rtems_HEADERS += include/rtems/rtc.h
include_rtems_HEADERS += include/rtems/watchdogdrv.h

## should be conditional and only installed for PowerPC, x86, and ARM
include_rtems_HEADERS += include/rtems/irq.h

include_rtems_HEADERS += include/rtems/irq-extension.h

## libfs
include_rtems_HEADERS += libfs/src/imfs/imfs.h

## POSIX FIFO/pipe
include_rtems_HEADERS += libfs/src/pipe/pipe.h

## devfs
include_rtems_HEADERS += libfs/src/devfs/devfs.h

if LIBDOSFS
include_rtems_HEADERS += libfs/src/dosfs/dosfs.h
endif

# RFS
include_rtems_HEADERS += libfs/src/rfs/rtems-rfs.h
include_rtems_HEADERS += libfs/src/rfs/rtems-rfs-format.h
include_rtems_HEADERS += libfs/src/rfs/rtems-rfs-shell.h

include_rtems_rfsdir = $(include_rtemsdir)/rfs
include_rtems_rfs_HEADERS =
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-bitmaps.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-block-pos.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-block.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-buffer.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-data.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-dir.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-dir-hash.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-file.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-file-system-fwd.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-file-system.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-group.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-inode.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-link.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-mutex.h
include_rtems_rfs_HEADERS += libfs/src/rfs/rtems-rfs-trace.h

# JFFS2
include_rtems_HEADERS += libfs/src/jffs2/include/rtems/jffs2.h

## libblock
include_rtems_HEADERS += libblock/include/rtems/bdbuf.h
include_rtems_HEADERS += libblock/include/rtems/blkdev.h
include_rtems_HEADERS += libblock/include/rtems/diskdevs.h
include_rtems_HEADERS += libblock/include/rtems/flashdisk.h
include_rtems_HEADERS += libblock/include/rtems/ramdisk.h
include_rtems_HEADERS += libblock/include/rtems/nvdisk.h
include_rtems_HEADERS += libblock/include/rtems/nvdisk-sram.h
include_rtems_HEADERS += libblock/include/rtems/sparse-disk.h
include_rtems_HEADERS += libblock/include/rtems/ide_part_table.h
include_rtems_HEADERS += libblock/include/rtems/bdpart.h
include_rtems_HEADERS += libblock/include/rtems/media.h

## ftpd
if LIBNETWORKING
include_rtems_HEADERS += ftpd/ftpd.h
endif

## bspcmdline
include_rtems_HEADERS += libmisc/bspcmdline/bspcmdline.h

## capture
include_rtems_HEADERS += libmisc/capture/capture.h
include_rtems_HEADERS += libmisc/capture/capture-cli.h
include_rtems_HEADERS += libmisc/capture/captureimpl.h

# tracing headers
include_rtems_tracedir = $(include_rtemsdir)/trace
include_rtems_trace_HEADERS =
include_rtems_trace_HEADERS += libmisc/capture/rtems-trace-buffer-vars.h

## cpuuse
include_rtems_HEADERS += libmisc/cpuuse/cpuuse.h

## devnull
include_rtems_HEADERS += libmisc/devnull/devnull.h
include_rtems_HEADERS += libmisc/devnull/devzero.h

## dumpbuf
include_rtems_HEADERS += libmisc/dumpbuf/dumpbuf.h

## monitor
include_rtems_HEADERS += libmisc/monitor/monitor.h

## fb
include_rtems_HEADERS += libmisc/fb/fb.h
include_rtems_HEADERS += libmisc/fb/mw_uid.h

## mouse
include_rtems_HEADERS += libmisc/mouse/mouse_parser.h
include_rtems_HEADERS += libmisc/mouse/serial_mouse.h

## libqos
include_rtems_HEADERS += libqos/qreslib.h

## redirector
include_rtems_HEADERS += libmisc/redirector/stdio-redirect.h
## shell
if LIBSHELL
include_rtems_HEADERS += libmisc/shell/shell.h
include_rtems_HEADERS += libmisc/shell/shellconfig.h
endif

## stringto
include_rtems_HEADERS += libmisc/stringto/stringto.h

## testsupport
include_rtems_HEADERS += libmisc/testsupport/test.h

## i2c
include_rtems_HEADERS += libi2c/libi2c.h

## serdbg
if LIBSERDBG
include_rtems_HEADERS += libmisc/serdbg/serdbgcnf.h
include_rtems_HEADERS += libmisc/serdbg/serdbg.h
include_rtems_HEADERS += libmisc/serdbg/termios_printk_cnf.h
include_rtems_HEADERS += libmisc/serdbg/termios_printk.h
endif

## stackchk
include_rtems_HEADERS += libmisc/stackchk/stackchk.h

## libuntar
include_rtems_HEADERS += libmisc/untar/untar.h

## fsmount
include_rtems_HEADERS += libmisc/fsmount/fsmount.h

## Driver manager
if LIBDRVMGR
include_drvmgrdir = $(includedir)/drvmgr
include_drvmgr_HEADERS = libdrvmgr/drvmgr.h
include_drvmgr_HEADERS += libdrvmgr/drvmgr_confdefs.h
include_drvmgr_HEADERS += libdrvmgr/drvmgr_list.h
endif

## HACK: doxygen filter.
EXTRA_DIST = doxy-filter

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
