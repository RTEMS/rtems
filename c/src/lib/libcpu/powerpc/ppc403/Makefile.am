##
## $Id$
##

CLEANFILES =
EXTRA_DIST =
noinst_PROGRAMS =

include $(top_srcdir)/../../../automake/compile.am

if ppc403
## clock
noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/clock.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## console
noinst_PROGRAMS += console.rel
if ppc405
console_rel_SOURCES = console/console405.c
else
console_rel_SOURCES = console/console.c
endif
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## ictrl
include_HEADERS = ictrl/ictrl.h

noinst_PROGRAMS += ictrl.rel
ictrl_rel_SOURCES = ictrl/ictrl.c ictrl/ictrl.h
ictrl_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
ictrl_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## timer
noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## tty_drv
if ppc405
include_HEADERS += tty_drv/tty_drv.h

noinst_PROGRAMS += tty_drv.rel
tty_drv_rel_SOURCES = tty_drv/tty_drv.c tty_drv/tty_drv.h
tty_drv_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
tty_drv_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

## vectors
noinst_PROGRAMS += vectors.rel
vectors_rel_SOURCES = vectors/vectors.S vectors/align_h.S
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

## --
all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

if ppc403

$(PROJECT_INCLUDE)/ictrl.h: ictrl/ictrl.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/ictrl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ictrl.h

if ppc405
$(PROJECT_INCLUDE)/tty_drv.h: tty_drv/tty_drv.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tty_drv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tty_drv.h
endif
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
