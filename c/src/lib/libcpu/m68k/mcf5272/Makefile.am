##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am

CLEANFILES =
EXTRA_PROGRAMS =
noinst_DATA =

if mcf5272

# include
include_mcf5272dir = $(includedir)/mcf5272
include_mcf5272_HEADERS = include/mcf5272.h 

## clock
EXTRA_PROGRAMS += clock.rel
CLEANFILES += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += clock_g.rel
CLEANFILES += clock_g.rel
clock_g_rel_SOURCES = $(clock_rel_SOURCES)
clock_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
clock_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += clock$(LIB_VARIANT).rel

## console
#EXTRA_PROGRAMS += console.rel
#CLEANFILES += console.rel
#console_rel_SOURCES = console/console-io.c ../../../../libbsp/shared/console-polled.c
#console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
#console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

#EXTRA_PROGRAMS += console_g.rel
#CLEANFILES += console_g.rel
#console_g_rel_SOURCES = $(console_rel_SOURCES)
#console_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
#console_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
#
#noinst_DATA += console$(LIB_VARIANT).rel

## timer
EXTRA_PROGRAMS += timer.rel
CLEANFILES += timer.rel
timer_rel_SOURCES = timer/timer.c timer/timerisr.S
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += timer_g.rel
CLEANFILES += timer_g.rel
timer_g_rel_SOURCES = $(timer_rel_SOURCES)
timer_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
timer_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += timer$(LIB_VARIANT).rel
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

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
