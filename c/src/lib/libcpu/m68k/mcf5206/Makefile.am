##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am

noinst_PROGRAMS =

if mcf5206

# include
include_mcf5206dir = $(includedir)/mcf5206
include_mcf5206_HEADERS = include/mcf5206e.h include/mcfmbus.h \
    include/mcfuart.h

## clock
noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## mcfuart
noinst_PROGRAMS += mcfuart.rel
mcfuart_rel_SOURCES = console/mcfuart.c
mcfuart_rel_CPPFLAGS = $(AM_CPPFLAGS)
mcfuart_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## mbus
noinst_PROGRAMS += mbus.rel
mbus_rel_SOURCES = mbus/mcfmbus.c
mbus_rel_CPPFLAGS = $(AM_CPPFLAGS)
mbus_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## timer
noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c timer/timerisr.S
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if mcf5206
$(PROJECT_INCLUDE)/mcf5206/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/mcf5206
	@: > $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)

$(PROJECT_INCLUDE)/mcf5206/mcf5206e.h: include/mcf5206e.h $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mcf5206/mcf5206e.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5206/mcf5206e.h

$(PROJECT_INCLUDE)/mcf5206/mcfmbus.h: include/mcfmbus.h $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mcf5206/mcfmbus.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5206/mcfmbus.h

$(PROJECT_INCLUDE)/mcf5206/mcfuart.h: include/mcfuart.h $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mcf5206/mcfuart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5206/mcfuart.h
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
