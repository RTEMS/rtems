##
##  $Id$
##

include $(top_srcdir)/automake/compile.am

EXTRA_DIST = README

noinst_LIBRARIES =

## capture
EXTRA_DIST += capture/README

noinst_LIBRARIES += libcapture.a
libcapture_a_SOURCES = capture/capture.c capture/capture-cli.c \
    capture/capture.h capture/capture-cli.h

## cpuuse
EXTRA_DIST += cpuuse/README

noinst_LIBRARIES += libcpuuse.a
libcpuuse_a_SOURCES = cpuuse/cpuusagereport.c cpuuse/cpuusagereset.c \
    cpuuse/cpuuse.h cpuuse/cpuusagedata.c

## devnull

noinst_LIBRARIES += libdevnull.a
libdevnull_a_SOURCES = devnull/devnull.c devnull/devnull.h

## dummy
EXTRA_DIST += dummy/README

noinst_LIBRARIES += libdummy.a
libdummy_a_SOURCES = dummy/dummy.c

## dumpbuf
noinst_LIBRARIES += libdumpbuf.a
libdumpbuf_a_SOURCES = dumpbuf/dumpbuf.c dumpbuf/dumpbuf.h

## monitor
noinst_LIBRARIES += libmonitor.a
libmonitor_a_SOURCES = monitor/mon-command.c monitor/mon-symbols.c \
    monitor/mon-prmisc.c monitor/mon-monitor.c monitor/mon-object.c \
    monitor/mon-server.c monitor/mon-task.c monitor/mon-queue.c \
    monitor/mon-driver.c monitor/mon-itask.c monitor/mon-extension.c \
    monitor/mon-manager.c monitor/mon-config.c monitor/mon-part.c \
    monitor/mon-region.c monitor/mon-sema.c monitor/symbols.h  \
    monitor/monitor.h
if LIBNETWORKING
libmonitor_a_SOURCES += monitor/mon-network.c
endif
if HAS_MP
libmonitor_a_SOURCES += monitor/mon-mpci.c
endif

EXTRA_DIST += monitor/README

## mw-fb
noinst_LIBRARIES += libmw-fb.a
libmw_fb_a_SOURCES = mw-fb/mw_fb.c mw-fb/mw_uid.c mw-fb/mw_fb.h \
    mw-fb/mw_uid.h

## shell

if LIBSHELL
noinst_LIBRARIES += libshell.a
if LIBNETWORKING
shell_mount_fs = shell/main_mount_ftp.c shell/main_mount_nfs.c \
    shell/main_mount_tftp.c
endif
libshell_a_SOURCES = shell/cat_file.c shell/cmds.c shell/internal.h \
    shell/main_alias.c shell/main_cat.c shell/main_cd.c \
    shell/main_chdir.c shell/main_chmod.c shell/main_chroot.c \
    shell/main_cpuuse.c shell/main_date.c shell/main_dir.c \
    shell/main_exit.c shell/main_help.c shell/main_id.c \
    shell/main_logoff.c shell/main_ls.c shell/main_mallocinfo.c \
    shell/main_mdump.c shell/main_medit.c shell/main_mfill.c \
    shell/main_mkdir.c shell/main_mount.c  $(shell_mount_fs) \
    shell/main_mount_msdos.c shell/main_mmove.c shell/main_msdosfmt.c \
    shell/main_mwdump.c shell/main_perioduse.c shell/main_pwd.c \
    shell/main_rm.c shell/main_rmdir.c shell/main_stackuse.c \
    shell/main_tty.c shell/main_umask.c shell/main_unmount.c \
    shell/main_blksync.c shell/main_whoami.c shell/shell.c \
    shell/shell_cmdset.c shell/shellconfig.c shell/shellconfig.h \
    shell/shell.h shell/shell_makeargs.c shell/str2int.c shell/write_file.c
endif

EXTRA_DIST += shell/README

## serdbg

if LIBSERDBG
noinst_LIBRARIES += libserdbg.a
libserdbg_a_SOURCES = serdbg/serdbg.c serdbg/serdbgio.c \
    serdbg/termios_printk.c serdbg/serdbgcnf.h serdbg/serdbg.h \
    serdbg/termios_printk_cnf.h serdbg/termios_printk.h
endif

EXTRA_DIST += serdbg/README

## stackchk
noinst_LIBRARIES += libstackchk.a
libstackchk_a_SOURCES = stackchk/check.c stackchk/internal.h \
    stackchk/stackchk.h

EXTRA_DIST += stackchk/README

## libuntar
noinst_LIBRARIES += libuntar.a
libuntar_a_SOURCES = untar/untar.c untar/untar.h

EXTRA_DIST += untar/README

## fsmount
noinst_LIBRARIES += libfsmount.a
libfsmount_a_SOURCES = fsmount/fsmount.c fsmount/fsmount.h

EXTRA_DIST += fsmount/README

## ---
include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
