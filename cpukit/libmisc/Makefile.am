##
##  $Id$
##

include $(top_srcdir)/automake/compile.am

EXTRA_DIST = README

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS =
noinst_LIBRARIES =
CLEANFILES =

## capture
EXTRA_DIST += capture/README
include_rtems_HEADERS += capture/capture.h capture/capture-cli.h

noinst_LIBRARIES += libcapture.a
libcapture_a_SOURCES = capture/capture.c capture/capture-cli.c
libcapture_a_CPPFLAGS = $(AM_CPPFLAGS)

## cpuuse
EXTRA_DIST += cpuuse/README
include_rtems_HEADERS += cpuuse/cpuuse.h

noinst_LIBRARIES += libcpuuse.a
libcpuuse_a_SOURCES = cpuuse/cpuuse.c
libcpuuse_a_CPPFLAGS = $(AM_CPPFLAGS)

## devnull
include_rtems_HEADERS += devnull/devnull.h

noinst_LIBRARIES += libdevnull.a
libdevnull_a_SOURCES = devnull/devnull.c
libdevnull_a_CPPFLAGS = $(AM_CPPFLAGS)

## dummy
EXTRA_DIST += dummy/README

noinst_LIBRARIES += libdummy.a
libdummy_a_SOURCES = dummy/dummy.c
libdummy_a_CPPFLAGS = $(AM_CPPFLAGS)

## dumpbuf
include_rtems_HEADERS += dumpbuf/dumpbuf.h

noinst_LIBRARIES += libdumpbuf.a
libdumpbuf_a_SOURCES = dumpbuf/dumpbuf.c
libdumpbuf_a_CPPFLAGS = $(AM_CPPFLAGS)

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

noinst_LIBRARIES += libmonitor.a
libmonitor_a_CPPFLAGS = $(AM_CPPFLAGS)

EXTRA_DIST += monitor/README

## mw-fb
include_rtems_HEADERS += mw-fb/mw_fb.h mw-fb/mw_uid.h

noinst_LIBRARIES += libmw-fb.a
libmw_fb_a_SOURCES = mw-fb/mw_fb.c mw-fb/mw_uid.c
libmw_fb_a_CPPFLAGS = $(AM_CPPFLAGS)

## shell

if LIBSHELL
include_rtems_HEADERS += shell/shell.h

noinst_LIBRARIES += libshell.a
libshell_a_SOURCES = shell/cmds.c shell/shell.c
libshell_a_CPPFLAGS = $(AM_CPPFLAGS)
endif

EXTRA_DIST += shell/README

## rtmonuse
include_rtems_HEADERS += rtmonuse/rtmonuse.h

noinst_LIBRARIES += librtmonuse.a
librtmonuse_a_SOURCES = rtmonuse/rtmonuse.c
librtmonuse_a_CPPFLAGS = $(AM_CPPFLAGS)

## serdbg

if LIBSERDBG
include_rtems_HEADERS += serdbg/serdbgcnf.h serdbg/serdbg.h \
    serdbg/termios_printk_cnf.h serdbg/termios_printk.h

noinst_LIBRARIES += libserdbg.a
libserdbg_a_SOURCES = serdbg/serdbg.c serdbg/serdbgio.c \
    serdbg/termios_printk.c
libserdbg_a_CPPFLAGS = $(AM_CPPFLAGS)
endif

EXTRA_DIST += serdbg/README

## stackchk

include_rtems_HEADERS += stackchk/stackchk.h

noinst_LIBRARIES += libstackchk.a
libstackchk_a_SOURCES = stackchk/check.c stackchk/internal.h
libstackchk_a_CPPFLAGS = $(AM_CPPFLAGS)

EXTRA_DIST += stackchk/README

## libuntar

include_rtems_HEADERS += untar/untar.h

noinst_LIBRARIES += libuntar.a
libuntar_a_SOURCES = untar/untar.c
libuntar_a_CPPFLAGS = $(AM_CPPFLAGS)

EXTRA_DIST += untar/README

## fsmount
include_rtems_HEADERS += fsmount/fsmount.h

noinst_LIBRARIES += libfsmount.a
libfsmount_a_SOURCES = fsmount/fsmount.c
libfsmount_a_CPPFLAGS = $(AM_CPPFLAGS)

EXTRA_DIST += fsmount/README

## ---

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

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
$(PROJECT_INCLUDE)/rtems/serdbgcnf.h: serdbg/serdbgcnf.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/serdbgcnf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/serdbgcnf.h

$(PROJECT_INCLUDE)/rtems/serdbg.h: serdbg/serdbg.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/serdbg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/serdbg.h

$(PROJECT_INCLUDE)/rtems/termios_printk_cnf.h: serdbg/termios_printk_cnf.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/termios_printk_cnf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/termios_printk_cnf.h

$(PROJECT_INCLUDE)/rtems/termios_printk.h: serdbg/termios_printk.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/termios_printk.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/termios_printk.h
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

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
