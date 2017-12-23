ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

EXTRA_DIST =

noinst_PROGRAMS =

## cache
noinst_PROGRAMS += shared/cache.rel
shared_cache_rel_SOURCES = shared/cache/cache.c \
    ../shared/src/cache_manager.c shared/cache/cache_.h
shared_cache_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/shared/cache
shared_cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## interrupts

if rm52xx
interrupts_CPPFLAGS = -DRM52XX
endif

if tx39
interrupts_CPPFLAGS = -DTX39
endif

if tx49
interrupts_CPPFLAGS = -DTX49
endif

noinst_PROGRAMS += shared/interrupts.rel
shared_interrupts_rel_SOURCES = shared/interrupts/installisrentries.c \
    shared/interrupts/isr_entries.S shared/interrupts/isr_entries.h
shared_interrupts_rel_CPPFLAGS = $(AM_CPPFLAGS) $(interrupts_CPPFLAGS)
shared_interrupts_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
if tx39
endif

if tx49
noinst_PROGRAMS += tx49/timer.rel
tx49_timer_rel_SOURCES = timer/timer.c timer/gettime.S
tx49_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
tx49_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if au1x00
endif

if rm52xx
noinst_PROGRAMS += rm52xx/timer.rel
rm52xx_timer_rel_SOURCES = timer/timer.c timer/gettime.S
rm52xx_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
rm52xx_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(top_srcdir)/../../../automake/local.am
