##
##  $Id$
##

include $(top_srcdir)/automake/amcompile.am

EXTRA_DIST = README

AM_CPPFLAGS += -I$(top_builddir)

include_rtemsdir = $(includedir)/rtems
include_HEADERS =
include_rtems_HEADERS =
EXTRA_LIBRARIES =
TMP_LIBS =

## capture

libcapture_C_FILES = capture/capture.c capture/capture-cli.c
include_rtems_HEADERS += capture/capture.h capture/capture-cli.h

EXTRA_LIBRARIES += o-optimize/libcapture.a
o_optimize_libcapture_a_SOURCES = $(libcapture_C_FILES)
o_optimize_libcapture_a_CPPFLAGS = $(AM_CPPFLAGS) \
    $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libcapture.a
o_debug_libcapture_a_SOURCES = $(libcapture_C_FILES)
o_debug_libcapture_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += $(ARCH)/libcapture.a

## cpuuse

libcpuuse_C_FILES = cpuuse/cpuuse.c
include_rtems_HEADERS += cpuuse/cpuuse.h

EXTRA_LIBRARIES += o-optimize/libcpuuse.a
o_optimize_libcpuuse_a_SOURCES = $(libcpuuse_C_FILES)
o_optimize_libcpuuse_a_CPPFLAGS = $(AM_CPPFLAGS) \
    $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libcpuuse.a
o_debug_libcpuuse_a_SOURCES = $(libcpuuse_C_FILES)
o_debug_libcpuuse_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += $(ARCH)/libcpuuse.a

## devnull

libdevnull_C_FILES = devnull/devnull.c
include_rtems_HEADERS += devnull/devnull.h

EXTRA_LIBRARIES += o-optimize/libdevnull.a
o_optimize_libdevnull_a_SOURCES = $(libdevnull_C_FILES)
o_optimize_libdevnull_a_CPPFLAGS = $(AM_CPPFLAGS) \
    $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libdevnull.a
o_debug_libdevnull_a_SOURCES = $(libdevnull_C_FILES)
o_debug_libdevnull_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += $(ARCH)/libdevnull.a

## dummy

libdummy_C_FILES = dummy/dummy.c
EXTRA_DIST += dummy/README

EXTRA_LIBRARIES += o-optimize/libdummy.a
o_optimize_libdummy_a_SOURCES = $(libdummy_C_FILES)
o_optimize_libdummy_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libdummy.a
o_debug_libdummy_a_SOURCES = $(libdummy_C_FILES)
o_debug_libdummy_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += $(ARCH)/libdummy.a

## dumpbuf

libdumpbuf_C_FILES = dumpbuf/dumpbuf.c
include_rtems_HEADERS += dumpbuf/dumpbuf.h

EXTRA_LIBRARIES += o-optimize/libdumpbuf.a
o_optimize_libdumpbuf_a_SOURCES = $(libdumpbuf_C_FILES)
o_optimize_libdumpbuf_a_CPPFLAGS = $(AM_CPPFLAGS) \
    $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libdumpbuf.a
o_debug_libdumpbuf_a_SOURCES = $(libdumpbuf_C_FILES)
o_debug_libdumpbuf_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += $(ARCH)/libdumpbuf.a

## monitor

include_rtems_HEADERS += monitor/monitor.h

libmonitor_C_FILES = monitor/mon-command.c monitor/mon-symbols.c \
    monitor/mon-prmisc.c monitor/mon-monitor.c monitor/mon-object.c \
    monitor/mon-server.c monitor/mon-task.c monitor/mon-queue.c \
    monitor/mon-driver.c monitor/mon-dname.c monitor/mon-itask.c \
    monitor/mon-extension.c monitor/mon-manager.c monitor/mon-config.c \
    monitor/symbols.h
if HAS_MP
libmonitor_C_FILES += monitor/mon-mpci.c
endif

EXTRA_LIBRARIES += o-optimize/libmonitor.a
o_optimize_libmonitor_a_SOURCES = $(libmonitor_C_FILES)
o_optimize_libmonitor_a_CPPFLAGS = $(AM_CPPFLAGS) \
    $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libmonitor.a
o_debug_libmonitor_a_SOURCES = $(libmonitor_C_FILES)
o_debug_libmonitor_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

EXTRA_DIST += monitor/README

TMP_LIBS += $(ARCH)/libmonitor.a

## mw-fb

libmw_fb_C_FILES = mw-fb/mw_fb.c mw-fb/mw_uid.c
include_rtems_HEADERS += mw-fb/mw_fb.h mw-fb/mw_uid.h

EXTRA_LIBRARIES += o-optimize/libmw-fb.a
o_optimize_libmw_fb_a_SOURCES = $(libmw_fb_C_FILES)
o_optimize_libmw_fb_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libmw-fb.a
o_debug_libmw_fb_a_SOURCES = $(libmw_fb_C_FILES)
o_debug_libmw_fb_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += $(ARCH)/libmw-fb.a

## shell

if LIBSHELL
include_rtems_HEADERS += shell/shell.h
libshell_C_FILES = shell/cmds.c shell/shell.c

EXTRA_LIBRARIES += o-optimize/libshell.a
o_optimize_libshell_a_SOURCES = $(libshell_C_FILES)
o_optimize_libshell_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libshell.a
o_debug_libshell_a_SOURCES = $(libshell_C_FILES)
o_debug_libshell_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += $(ARCH)/libshell.a
endif

EXTRA_DIST += shell/README

## rtmonuse

librtmonuse_C_FILES = rtmonuse/rtmonuse.c
include_rtems_HEADERS += rtmonuse/rtmonuse.h

EXTRA_LIBRARIES += o-optimize/librtmonuse.a
o_optimize_librtmonuse_a_SOURCES = $(librtmonuse_C_FILES)
o_optimize_librtmonuse_a_CPPFLAGS = $(AM_CPPFLAGS) \
    $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/librtmonuse.a
o_debug_librtmonuse_a_SOURCES = $(librtmonuse_C_FILES)
o_debug_librtmonuse_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += $(ARCH)/librtmonuse.a

## serdbg

if LIBSERDBG
include_HEADERS += serdbg/serdbgcnf.h serdbg/serdbg.h \
    serdbg/termios_printk_cnf.h serdbg/termios_printk.h

libserdbg_C_FILES = serdbg/serdbg.c serdbg/serdbgio.c \
    serdbg/termios_printk.c

EXTRA_LIBRARIES += o-optimize/libserdbg.a
o_optimize_libserdbg_a_SOURCES = $(libserdbg_C_FILES)
o_optimize_libserdbg_a_CPPFLAGS = $(AM_CPPFLAGS) \
    $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libserdbg.a
o_debug_libserdbg_a_SOURCES = $(libserdbg_C_FILES)
o_debug_libserdbg_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += $(ARCH)/libserdbg.a
endif

EXTRA_DIST += serdbg/README

## stackchk

include_rtems_HEADERS += stackchk/stackchk.h
libstackchk_C_FILES = stackchk/check.c stackchk/internal.h

EXTRA_LIBRARIES += o-optimize/libstackchk.a
o_optimize_libstackchk_a_SOURCES = $(libstackchk_C_FILES)
o_optimize_libstackchk_a_CPPFLAGS = $(AM_CPPFLAGS) \
    $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libstackchk.a
o_debug_libstackchk_a_SOURCES = $(libstackchk_C_FILES)
o_debug_libstackchk_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += $(ARCH)/libstackchk.a
EXTRA_DIST += stackchk/README

## libuntar

include_rtems_HEADERS += untar/untar.h
libuntar_C_FILES = untar/untar.c

EXTRA_LIBRARIES += o-optimize/libuntar.a
o_optimize_libuntar_a_SOURCES = $(libuntar_C_FILES)
o_optimize_libuntar_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libuntar.a
o_debug_libuntar_a_SOURCES = $(libuntar_C_FILES)
o_debug_libuntar_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += $(ARCH)/libuntar.a

EXTRA_DIST += untar/README

## fsmount

libfsmount_C_FILES = fsmount/fsmount.c
include_rtems_HEADERS += fsmount/fsmount.h

EXTRA_LIBRARIES += o-optimize/libfsmount.a
o_optimize_libfsmount_a_SOURCES = $(libfsmount_C_FILES)
o_optimize_libfsmount_a_CPPFLAGS = $(AM_CPPFLAGS) \
    $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libfsmount.a
o_debug_libfsmount_a_SOURCES = $(libfsmount_C_FILES)
o_debug_libfsmount_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += $(ARCH)/libfsmount.a

EXTRA_DIST += fsmount/README

## ---

all-local: $(PREINSTALL_FILES) $(TMP_LIBS)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/capture.h: capture/capture.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/capture.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/capture.h

$(PROJECT_INCLUDE)/rtems/capture-cli.h: capture/capture-cli.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/capture-cli.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/capture-cli.h

$(PROJECT_INCLUDE)/rtems/cpuuse.h: cpuuse/cpuuse.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/cpuuse.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/cpuuse.h

$(PROJECT_INCLUDE)/rtems/devnull.h: devnull/devnull.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/devnull.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/devnull.h

$(PROJECT_INCLUDE)/rtems/dumpbuf.h: dumpbuf/dumpbuf.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/dumpbuf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/dumpbuf.h

$(PROJECT_INCLUDE)/rtems/monitor.h: monitor/monitor.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/monitor.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/monitor.h

$(PROJECT_INCLUDE)/rtems/mw_fb.h: mw-fb/mw_fb.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/mw_fb.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/mw_fb.h

$(PROJECT_INCLUDE)/rtems/mw_uid.h: mw-fb/mw_uid.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/mw_uid.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/mw_uid.h

if LIBSHELL
$(PROJECT_INCLUDE)/rtems/shell.h: shell/shell.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/shell.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/shell.h
endif

$(PROJECT_INCLUDE)/rtems/rtmonuse.h: rtmonuse/rtmonuse.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/rtmonuse.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/rtmonuse.h

if LIBSERDBG
$(PROJECT_INCLUDE)/serdbgcnf.h: serdbg/serdbgcnf.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/serdbgcnf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/serdbgcnf.h

$(PROJECT_INCLUDE)/serdbg.h: serdbg/serdbg.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/serdbg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/serdbg.h

$(PROJECT_INCLUDE)/termios_printk_cnf.h: serdbg/termios_printk_cnf.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/termios_printk_cnf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/termios_printk_cnf.h

$(PROJECT_INCLUDE)/termios_printk.h: serdbg/termios_printk.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/termios_printk.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/termios_printk.h
endif

$(PROJECT_INCLUDE)/rtems/stackchk.h: stackchk/stackchk.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/stackchk.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/stackchk.h

$(PROJECT_INCLUDE)/rtems/untar.h: untar/untar.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/untar.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/untar.h

$(PROJECT_INCLUDE)/rtems/fsmount.h: fsmount/fsmount.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/fsmount.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/fsmount.h

include $(top_srcdir)/automake/local.am
