##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am

noinst_PROGRAMS =

if mcf5272

# include
include_mcf5272dir = $(includedir)/mcf5272
include_mcf5272_HEADERS = include/mcf5272.h

## clock
noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## console
#EXTRA_PROGRAMS += console.rel
##console_rel_SOURCES = console/console-io.c ../../../../libbsp/shared/console-polled.c
#console_rel_CPPFLAGS = $(AM_CPPFLAGS)
#console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

#EXTRA_PROGRAMS += console_g.rel
#

#

## timer
noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c timer/timerisr.S
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if mcf5272
$(PROJECT_INCLUDE)/mcf5272/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/mcf5272
	@: > $(PROJECT_INCLUDE)/mcf5272/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/mcf5272/$(dirstamp)

$(PROJECT_INCLUDE)/mcf5272/mcf5272.h: include/mcf5272.h $(PROJECT_INCLUDE)/mcf5272/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mcf5272/mcf5272.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5272/mcf5272.h
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
