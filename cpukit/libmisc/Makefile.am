##
##  $Id$
##

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

EXTRA_DIST = README

AM_CPPFLAGS += -I$(top_builddir)

include_rtemsdir = $(includedir)/rtems
include_HEADERS =
include_rtems_HEADERS =
noinst_HEADERS =

## capture

libcapture_C_FILES = capture/capture.c capture/capture-cli.c
libcapture_OBJS = $(C_FILES:capture/%.c=capture/$(ARCH)/%.$(OBJEXT))

include_rtems_HEADERS += capture/capture.h capture/capture-cli.h

capture/$(ARCH):
	@$(mkinstalldirs) $@
capture/$(ARCH)/%.$(OBJEXT): capture/%.c
	${COMPILE} -o $@ -c $<
capture/$(ARCH)/libcapture.a: $(libcapture_OBJS)
	$(make-library)

LIBS += capture/$(ARCH) capture/$(ARCH)/libcapture.a

EXTRA_DIST += capture/README capture/capture.c capture/capture-cli.c

## cpuuse

libcpuuse_C_FILES = cpuuse/cpuuse.c
libcpuuse_OBJS = $(libcpuuse_C_FILES:cpuuse/%.c=cpuuse/$(ARCH)/%.$(OBJEXT))

include_rtems_HEADERS += cpuuse/cpuuse.h

cpuuse/$(ARCH):
	@$(mkinstalldirs) $@
cpuuse/$(ARCH)/%.$(OBJEXT): cpuuse/%.c
	${COMPILE} -o $@ -c $<
cpuuse/$(ARCH)/libcpuuse.a: $(libcpuuse_OBJS)
	$(make-library)

LIBS += cpuuse/$(ARCH) cpuuse/$(ARCH)/libcpuuse.a
EXTRA_DIST += cpuuse/README cpuuse/cpuuse.c

## devnull

libdevnull_C_FILES = devnull/devnull.c
libdevnull_OBJS = $(libdevnull_C_FILES:devnull/%.c=devnull/$(ARCH)/%.$(OBJEXT))

include_rtems_HEADERS += devnull/devnull.h

devnull/$(ARCH):
	@$(mkinstalldirs) $@
devnull/$(ARCH)/%.$(OBJEXT): devnull/%.c
	${COMPILE} -o $@ -c $<
devnull/$(ARCH)/libdevnull.a: $(libdevnull_OBJS)
	$(make-library)

LIBS += devnull/$(ARCH) devnull/$(ARCH)/libdevnull.a

EXTRA_DIST += devnull/devnull.c

## dummy

libdummy_C_FILES = dummy/dummy.c
libdummy_OBJS = $(libdummy_C_FILES:dummy/%.c=dummy/$(ARCH)/%.$(OBJEXT))

dummy/$(ARCH):
	@$(mkinstalldirs) $@
dummy/$(ARCH)/%.$(OBJEXT): dummy/%.c
	${COMPILE} -o $@ -c $<
dummy/$(ARCH)/libdummy.a: $(libdummy_OBJS)
	$(make-library)

LIBS += dummy/$(ARCH) dummy/$(ARCH)/libdummy.a

EXTRA_DIST += dummy/README dummy/dummy.c

## dumpbuf

libdumpbuf_C_FILES = dumpbuf/dumpbuf.c
libdumpbuf_OBJS = $(libdumpbuf_C_FILES:dumpbuf/%.c=dumpbuf/$(ARCH)/%.$(OBJEXT))

include_rtems_HEADERS += dumpbuf/dumpbuf.h

dumpbuf/$(ARCH):
	@$(mkinstalldirs) $@
dumpbuf/$(ARCH)/%.$(OBJEXT): dumpbuf/%.c
	${COMPILE} -o $@ -c $<
dumpbuf/$(ARCH)/libdumpbuf.a: $(libdumpbuf_OBJS)
	$(make-library)

LIBS += dumpbuf/$(ARCH) dumpbuf/$(ARCH)/libdumpbuf.a

EXTRA_DIST += dumpbuf/dumpbuf.c

## monitor

# We only build multiprocessing related files if HAS_MP was defined
libmonitor_MP_C_FILES = monitor/mon-mpci.c

libmonitor_C_FILES = monitor/mon-command.c monitor/mon-symbols.c monitor/mon-prmisc.c monitor/mon-monitor.c \
    monitor/mon-object.c monitor/mon-server.c monitor/mon-task.c monitor/mon-queue.c monitor/mon-driver.c \
    monitor/mon-dname.c monitor/mon-itask.c monitor/mon-extension.c monitor/mon-manager.c monitor/mon-config.c

libmonitor_OBJS = $(libmonitor_C_FILES:monitor/%.c=monitor/$(ARCH)/%.$(OBJEXT))
if HAS_MP
libmonitor_OBJS += $(libmonitor_MP_C_FILES:monitor/%.c=monitor/$(ARCH)/%.$(OBJEXT))
endif

include_rtems_HEADERS += monitor/monitor.h

noinst_HEADERS += monitor/symbols.h

monitor/$(ARCH):
	@$(mkinstalldirs) $@
monitor/$(ARCH)/%.$(OBJEXT): monitor/%.c
	${COMPILE} -o $@ -c $<
monitor/$(ARCH)/libmonitor.a: $(libmonitor_OBJS)
	$(make-library)

LIBS += monitor/$(ARCH) monitor/$(ARCH)/libmonitor.a

EXTRA_DIST += monitor/README monitor/mon-command.c monitor/mon-config.c monitor/mon-dname.c monitor/mon-driver.c \
    monitor/mon-extension.c monitor/mon-itask.c monitor/mon-manager.c monitor/mon-monitor.c monitor/mon-mpci.c \
    monitor/mon-object.c monitor/mon-prmisc.c monitor/mon-queue.c monitor/mon-server.c monitor/mon-symbols.c \
    monitor/mon-task.c

## mw-fb

libmw_fb_C_FILES = mw-fb/mw_fb.c mw-fb/mw_uid.c
libmw_fb_OBJS = $(libmw_fb_C_FILES:mw-fb/%.c=mw-fb/${ARCH}/%.$(OBJEXT))

include_rtems_HEADERS += mw-fb/mw_fb.h mw-fb/mw_uid.h

mw-fb/$(ARCH):
	@$(mkinstalldirs) $@
mw-fb/$(ARCH)/%.$(OBJEXT): mw-fb/%.c
	${COMPILE} -o $@ -c $<
mw-fb/$(ARCH)/libmw-fb.a: $(libmw_fb_OBJS)
	$(make-library)

LIBS += mw-fb/$(ARCH) mw-fb/$(ARCH)/libmw-fb.a

EXTRA_DIST += $(libmw_fb_C_FILES)

## shell

libshell_C_FILES = shell/cmds.c shell/shell.c 
libshell_OBJS = $(libshell_C_FILES:shell/%.c=shell/$(ARCH)/%.$(OBJEXT))

if LIBSHELL
include_rtems_HEADERS += shell/shell.h
endif

shell/$(ARCH):
	@$(mkinstalldirs) $@
shell/$(ARCH)/%.$(OBJEXT): shell/%.c
	${COMPILE} -o $@ -c $<
shell/$(ARCH)/libshell.a: $(libshell_OBJS)
	$(make-library)

if LIBSHELL
LIBS += shell/$(ARCH) shell/$(ARCH)/libshell.a
endif

EXTRA_DIST += shell/README shell/shell.c shell/cmds.c

## rtmonuse

librtmonuse_C_FILES = rtmonuse/rtmonuse.c
librtmonuse_OBJS = $(librtmonuse_C_FILES:rtmonuse/%.c=rtmonuse/$(ARCH)/%.$(OBJEXT))

include_rtems_HEADERS += rtmonuse/rtmonuse.h

rtmonuse/$(ARCH):
	@$(mkinstalldirs) $@
rtmonuse/$(ARCH)/%.$(OBJEXT): rtmonuse/%.c
	${COMPILE} -o $@ -c $<
rtmonuse/$(ARCH)/librtmonuse.a: $(librtmonuse_OBJS)
	$(make-library)

LIBS += rtmonuse/$(ARCH) rtmonuse/$(ARCH)/librtmonuse.a

EXTRA_DIST += rtmonuse/rtmonuse.c

## serdbg

libserdbg_C_FILES = serdbg/serdbg.c serdbg/serdbgio.c serdbg/termios_printk.c
libserdbg_OBJS = $(libserdbg_C_FILES:serdbg/%.c=serdbg/$(ARCH)/%.$(OBJEXT))

if LIBSERDBG
include_HEADERS += serdbg/serdbgcnf.h serdbg/serdbg.h serdbg/termios_printk_cnf.h \
                  serdbg/termios_printk.h
endif

serdbg/$(ARCH):
	@$(mkinstalldirs) $@
serdbg/$(ARCH)/%.$(OBJEXT): serdbg/%.c
	${COMPILE} -o $@ -c $<
serdbg/$(ARCH)/libserdbg.a: $(libserdbg_OBJS)
	$(make-library)

if LIBSERDBG
LIBS += serdbg/$(ARCH) serdbg/$(ARCH)/libserdbg.a
endif

EXTRA_DIST += serdbg/README serdbg/serdbg.c serdbg/serdbgio.c serdbg/serdbgcnf.h serdbg/serdbg.h \
             serdbg/termios_printk.c serdbg/termios_printk.h serdbg/termios_printk_cnf.h

## stackchk

libstackchk_C_FILES = stackchk/check.c
libstackchk_OBJS = $(libstackchk_C_FILES:stackchk/%.c=stackchk/$(ARCH)/%.$(OBJEXT))

include_rtems_HEADERS += stackchk/stackchk.h

noinst_HEADERS += stackchk/internal.h

stackchk/$(ARCH):
	@$(mkinstalldirs) $@
stackchk/$(ARCH)/%.$(OBJEXT): stackchk/%.c
	${COMPILE} -o $@ -c $<
stackchk/$(ARCH)/libstackchk.a: $(libstackchk_OBJS)
	$(make-library)

LIBS += stackchk/$(ARCH) stackchk/$(ARCH)/libstackchk.a

EXTRA_DIST += stackchk/README stackchk/check.c

## libuntar

libuntar_C_FILES = untar/untar.c
libuntar_OBJS = $(libuntar_C_FILES:untar/%.c=untar/$(ARCH)/%.$(OBJEXT))

include_rtems_HEADERS += untar/untar.h

untar/$(ARCH):
	@$(mkinstalldirs) $@
untar/$(ARCH)/%.$(OBJEXT): untar/%.c
	${COMPILE} -o $@ -c $<
untar/$(ARCH)/libuntar.a: $(libuntar_OBJS)
	$(make-library)

LIBS += untar/$(ARCH) untar/$(ARCH)/libuntar.a

EXTRA_DIST += untar/README untar/untar.c

## fsmount

libfsmount_C_FILES = fsmount/fsmount.c
libfsmount_OBJS = $(libfsmount_C_FILES:fsmount/%.c=fsmount/$(ARCH)/%.$(OBJEXT))

include_rtems_HEADERS += fsmount/fsmount.h

fsmount/$(ARCH):
	@$(mkinstalldirs) $@
fsmount/$(ARCH)/%.$(OBJEXT): fsmount/%.c
	${COMPILE} -o $@ -c $<
fsmount/$(ARCH)/libfsmount.a: $(libfsmount_OBJS)
	$(make-library)

LIBS += fsmount/$(ARCH) fsmount/$(ARCH)/libfsmount.a

EXTRA_DIST += fsmount/README fsmount/fsmount.c

## ---

all-local: $(LIBS)
.PRECIOUS: $(LIBS)

CLEANDIRS = */$(ARCH)

include $(top_srcdir)/automake/local.am
