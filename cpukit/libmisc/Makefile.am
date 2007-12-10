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
libshell_a_SOURCES = shell/cat_file.c shell/cmd_alias.c shell/cmd_cat.c
    shell/cmd_cd.c shell/cmd_chdir.c shell/cmd_chmod.c shell/cmd_chroot.c
    shell/cmd_date.c shell/cmd_dir.c shell/cmd_exit.c shell/cmd_help.c
    shell/cmd_id.c shell/cmd_logoff.c shell/cmd_ls.c shell/cmd_mallocdump.c
    shell/cmd_mdump.c shell/cmd_medit.c shell/cmd_mfill.c shell/cmd_mkdir.c
    shell/cmd_mmove.c shell/cmd_mwdump.c shell/cmd_pwd.c shell/cmd_rm.c
    shell/cmd_rmdir.c shell/cmds.c shell/cmd_tty.c shell/cmd_umask.c
    shell/cmd_whoami.c shell/internal.h shell/shell.c shell/shellconfig.c
    shell/shellconfig.h shell/shell.h shell/str2int.c
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
