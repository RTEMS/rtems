##
##  $Id$
##

include $(top_srcdir)/automake/compile.am

EXTRA_DIST = README

AM_CPPFLAGS += -I$(top_builddir)

include_rtemsdir = $(includedir)/rtems
include_HEADERS =
include_rtems_HEADERS =
EXTRA_LIBRARIES =
TMP_LIBS =
CLEANFILES =

## capture
EXTRA_DIST += capture/README
include_rtems_HEADERS += capture/capture.h capture/capture-cli.h

EXTRA_LIBRARIES += libcapture.a
CLEANFILES += libcapture.a
libcapture_a_SOURCES = capture/capture.c capture/capture-cli.c
libcapture_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libcapture_g.a
CLEANFILES += libcapture_g.a
libcapture_g_a_SOURCES = $(libcapture_a_SOURCES)
libcapture_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += libcapture$(LIB_VARIANT).a

## cpuuse
EXTRA_DIST += cpuuse/README
include_rtems_HEADERS += cpuuse/cpuuse.h

EXTRA_LIBRARIES += libcpuuse.a
CLEANFILES += libcpuuse.a
libcpuuse_a_SOURCES = cpuuse/cpuuse.c
libcpuuse_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libcpuuse_g.a
CLEANFILES += libcpuuse_g.a
libcpuuse_g_a_SOURCES = $(libcpuuse_a_SOURCES)
libcpuuse_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += libcpuuse$(LIB_VARIANT).a

## devnull
include_rtems_HEADERS += devnull/devnull.h

EXTRA_LIBRARIES += libdevnull.a
CLEANFILES += libdevnull.a
libdevnull_a_SOURCES = devnull/devnull.c
libdevnull_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libdevnull_g.a
CLEANFILES += libdevnull_g.a
libdevnull_g_a_SOURCES = $(libdevnull_a_SOURCES)
libdevnull_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += libdevnull$(LIB_VARIANT).a

## dummy
EXTRA_DIST += dummy/README

EXTRA_LIBRARIES += libdummy.a
CLEANFILES += libdummy.a
libdummy_a_SOURCES = dummy/dummy.c
libdummy_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libdummy_g.a
CLEANFILES += libdummy_g.a
libdummy_g_a_SOURCES = $(libdummy_a_SOURCES)
libdummy_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += libdummy$(LIB_VARIANT).a

## dumpbuf
include_rtems_HEADERS += dumpbuf/dumpbuf.h

EXTRA_LIBRARIES += libdumpbuf.a
CLEANFILES += libdumpbuf.a
libdumpbuf_a_SOURCES = dumpbuf/dumpbuf.c
libdumpbuf_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libdumpbuf_g.a
CLEANFILES += libdumpbuf_g.a
libdumpbuf_g_a_SOURCES = $(libdumpbuf_a_SOURCES)
libdumpbuf_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += libdumpbuf$(LIB_VARIANT).a

## monitor

include_rtems_HEADERS += monitor/monitor.h
libmonitor_a_SOURCES = monitor/mon-command.c monitor/mon-symbols.c \
    monitor/mon-prmisc.c monitor/mon-monitor.c monitor/mon-object.c \
    monitor/mon-server.c monitor/mon-task.c monitor/mon-queue.c \
    monitor/mon-driver.c monitor/mon-dname.c monitor/mon-itask.c \
    monitor/mon-extension.c monitor/mon-manager.c monitor/mon-config.c \
    monitor/symbols.h
if HAS_MP
libmonitor_a_SOURCES += monitor/mon-mpci.c
endif

EXTRA_LIBRARIES += libmonitor.a
CLEANFILES += libmonitor.a
libmonitor_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libmonitor_g.a
CLEANFILES += libmonitor_g.a
libmonitor_g_a_SOURCES = $(libmonitor_a_SOURCES)
libmonitor_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

EXTRA_DIST += monitor/README

TMP_LIBS += libmonitor$(LIB_VARIANT).a

## mw-fb
include_rtems_HEADERS += mw-fb/mw_fb.h mw-fb/mw_uid.h

EXTRA_LIBRARIES += libmw-fb.a
CLEANFILES += libmw-fb.a
libmw_fb_a_SOURCES = mw-fb/mw_fb.c mw-fb/mw_uid.c
libmw_fb_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libmw-fb_g.a
CLEANFILES += libmw-fb_g.a
libmw_fb_g_a_SOURCES = $(libmw_fb_a_SOURCES)
libmw_fb_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += libmw-fb$(LIB_VARIANT).a

## shell

if LIBSHELL
include_rtems_HEADERS += shell/shell.h

EXTRA_LIBRARIES += libshell.a
CLEANFILES += libshell.a
libshell_a_SOURCES = shell/cmds.c shell/shell.c
libshell_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libshell_g.a
CLEANFILES += libshell_g.a
libshell_g_a_SOURCES = $(libshell_a_SOURCES)
libshell_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += libshell$(LIB_VARIANT).a
endif

EXTRA_DIST += shell/README

## rtmonuse
include_rtems_HEADERS += rtmonuse/rtmonuse.h

EXTRA_LIBRARIES += librtmonuse.a
CLEANFILES += librtmonuse.a
librtmonuse_a_SOURCES = rtmonuse/rtmonuse.c
librtmonuse_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += librtmonuse_g.a
CLEANFILES += librtmonuse_g.a
librtmonuse_g_a_SOURCES = $(librtmonuse_a_SOURCES)
librtmonuse_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += librtmonuse$(LIB_VARIANT).a

## serdbg

if LIBSERDBG
include_HEADERS += serdbg/serdbgcnf.h serdbg/serdbg.h \
    serdbg/termios_printk_cnf.h serdbg/termios_printk.h

EXTRA_LIBRARIES += libserdbg.a
CLEANFILES += libserdbg.a
libserdbg_a_SOURCES = serdbg/serdbg.c serdbg/serdbgio.c \
    serdbg/termios_printk.c
libserdbg_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libserdbg_g.a
CLEANFILES += libserdbg_g.a
libserdbg_g_a_SOURCES = $(libserdbg_a_SOURCES)
libserdbg_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += libserdbg$(LIB_VARIANT).a
endif

EXTRA_DIST += serdbg/README

## stackchk

include_rtems_HEADERS += stackchk/stackchk.h

EXTRA_LIBRARIES += libstackchk.a
CLEANFILES += libstackchk.a
libstackchk_a_SOURCES = stackchk/check.c stackchk/internal.h
libstackchk_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libstackchk_g.a
CLEANFILES += libstackchk_g.a
libstackchk_g_a_SOURCES = $(libstackchk_a_SOURCES)
libstackchk_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += libstackchk$(LIB_VARIANT).a
EXTRA_DIST += stackchk/README

## libuntar

include_rtems_HEADERS += untar/untar.h

EXTRA_LIBRARIES += libuntar.a
CLEANFILES += libuntar.a
libuntar_a_SOURCES = untar/untar.c
libuntar_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libuntar_g.a
CLEANFILES += libuntar_g.a
libuntar_g_a_SOURCES = $(libuntar_a_SOURCES)
libuntar_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += libuntar$(LIB_VARIANT).a

EXTRA_DIST += untar/README

## fsmount
include_rtems_HEADERS += fsmount/fsmount.h

EXTRA_LIBRARIES += libfsmount.a
CLEANFILES += libfsmount.a
libfsmount_a_SOURCES = fsmount/fsmount.c
libfsmount_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libfsmount_g.a
CLEANFILES += libfsmount_g.a
libfsmount_g_a_SOURCES = $(libfsmount_a_SOURCES)
libfsmount_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

TMP_LIBS += libfsmount$(LIB_VARIANT).a

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
