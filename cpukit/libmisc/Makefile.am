##
##

include $(top_srcdir)/automake/compile.am

EXTRA_DIST = README

noinst_LIBRARIES =

## bspcmdline
noinst_LIBRARIES += libbspcmdline.a
libbspcmdline_a_SOURCES = bspcmdline/bspcmdline_get.c \
    bspcmdline/bspcmdline_getparam.c bspcmdline/bspcmdline_getparamraw.c \
    bspcmdline/bspcmdline_getparamrhs.c

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
libdevnull_a_SOURCES = devnull/devnull.c devnull/devnull.h \
    devnull/devzero.c devnull/devzero.h

## dummy
EXTRA_DIST += dummy/README

noinst_LIBRARIES += libdummy.a
libdummy_a_SOURCES = dummy/dummy.c dummy/dummy-networking.c

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
    monitor/mon-region.c monitor/mon-sema.c monitor/mon-editor.c \
    monitor/symbols.h monitor/monitor.h
if LIBNETWORKING
libmonitor_a_SOURCES += monitor/mon-network.c
endif
if HAS_MP
libmonitor_a_SOURCES += monitor/mon-mpci.c
endif

EXTRA_DIST += monitor/README

## fb
noinst_LIBRARIES += libmw-fb.a
libmw_fb_a_SOURCES = fb/mw_print.c fb/mw_uid.c fb/fb.h fb/mw_uid.h

## mouse
noinst_LIBRARIES += libmouse.a
libmouse_a_SOURCES = mouse/mouse_parser.c mouse/serial_mouse.c
EXTRA_DIST += mouse/README

## shell
if LIBSHELL
noinst_LIBRARIES += libshell.a
libshell_a_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/shell
libshell_a_SOURCES = shell/cat_file.c shell/cmds.c shell/internal.h \
    shell/main_alias.c shell/main_cat.c shell/main_cd.c shell/cmp-ls.c \
    shell/main_chdir.c shell/main_chmod.c shell/main_chroot.c \
    shell/main_cp.c shell/main_cpuuse.c shell/main_date.c shell/main_dir.c \
    shell/main_echo.c shell/main_exit.c shell/main_halt.c shell/main_help.c \
    shell/main_id.c shell/main_logoff.c shell/main_ln.c shell/main_ls.c \
    shell/main_mallocinfo.c shell/main_mdump.c shell/main_medit.c \
    shell/main_mfill.c shell/main_mkdir.c shell/main_mount.c \
    shell/main_mmove.c shell/main_msdosfmt.c \
    shell/main_mv.c shell/main_perioduse.c \
    shell/main_pwd.c shell/main_rm.c shell/main_rmdir.c shell/main_sleep.c \
    shell/main_stackuse.c shell/main_tty.c shell/main_umask.c \
    shell/main_unmount.c shell/main_blksync.c shell/main_whoami.c \
    shell/shell.c shell/shell_cmdset.c shell/shell_getchar.c \
    shell/shell_getprompt.c shell/shellconfig.c \
    shell/shellconfig.h shell/shell.h shell/shell_makeargs.c \
    shell/filemode.c shell/pwcache.c shell/print-ls.c \
    shell/write_file.c shell/utils-cp.c  shell/utils-ls.c \
    shell/err.c shell/errx.c shell/verr.c shell/vis.c \
    shell/verrx.c shell/vwarn.c shell/vwarnx.c shell/warn.c shell/warnx.c \
    shell/fts.c shell/print_heapinfo.c shell/main_wkspaceinfo.c \
    shell/shell_script.c shell/login_prompt.c shell/login_check.c \
    shell/fdisk.c shell/main_rtc.c \
    shell/dd-args.c shell/main_dd.c shell/dd-conv.c shell/dd-conv_tab.c \
    shell/dd-misc.c shell/dd-position.c \
    shell/main_hexdump.c shell/hexdump-conv.c shell/hexdump-display.c \
    shell/hexdump-odsyntax.c shell/hexdump-parse.c shell/hexsyntax.c \
    shell/main_time.c shell/main_mknod.c \
    shell/main_setenv.c shell/main_getenv.c shell/main_unsetenv.c \
    shell/main_mkrfs.c shell/main_debugrfs.c \
    shell/main_lsof.c \
    shell/main_blkstats.c \
    shell/shell-wait-for-input.c

if LIBNETWORKING
libshell_a_SOURCES += \
    shell/main_ifconfig.c shell/main_route.c shell/main_netstats.c
endif
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

## stringto
noinst_LIBRARIES += libstringto.a
libstringto_a_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/stringto
libstringto_a_SOURCES = stringto/stringtodouble.c stringto/stringtofloat.c \
    stringto/stringtolongdouble.c \
    stringto/stringtoint.c stringto/stringtolong.c stringto/stringtolonglong.c \
    stringto/stringtopointer.c stringto/stringtounsignedint.c \
    stringto/stringtounsignedchar.c stringto/stringtounsignedlong.c \
    stringto/stringtounsignedlonglong.c

## fsmount
noinst_LIBRARIES += libfsmount.a
libfsmount_a_SOURCES = fsmount/fsmount.c fsmount/fsmount.h

## uuid
noinst_LIBRARIES += libuuid.a
libuuid_a_SOURCES = uuid/clear.c uuid/compare.c uuid/copy.c uuid/gen_uuid.c \
	uuid/isnull.c uuid/pack.c uuid/parse.c uuid/unpack.c uuid/unparse.c \
	uuid/uuid_time.c uuid/uuidd.h uuid/uuidP.h

## ---
include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
