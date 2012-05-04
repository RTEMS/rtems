ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

EXTRA_DIST =

noinst_PROGRAMS =

## cache
include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = ../shared/include/cache.h

noinst_PROGRAMS += shared/cache.rel
shared_cache_rel_SOURCES = shared/cache/cache.c ../shared/src/cache_aligned_malloc.c \
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

if mongoosev
interrupts_CPPFLAGS = -DMONGOOSEV
endif

noinst_PROGRAMS += shared/interrupts.rel
shared_interrupts_rel_SOURCES = shared/interrupts/installisrentries.c \
    shared/interrupts/isr_entries.S shared/interrupts/isr_entries.h
shared_interrupts_rel_CPPFLAGS = $(AM_CPPFLAGS) $(interrupts_CPPFLAGS)
shared_interrupts_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
include_libcpu_HEADERS += shared/interrupts/isr_entries.h

EXTRA_DIST += mongoosev/README
EXTRA_DIST += mongoosev/duart/README.mguart
if mongoosev
include_libcpu_HEADERS += mongoosev/include/mongoose-v.h

## mongoosev/duart
include_libchipdir = $(includedir)/libchip
include_libchip_HEADERS = mongoosev/duart/mg5uart.h

noinst_PROGRAMS += mongoosev/duart.rel
mongoosev_duart_rel_SOURCES = mongoosev/duart/mg5uart.c \
    mongoosev/duart/mg5uart.h mongoosev/duart/mg5uart_reg.c
mongoosev_duart_rel_CPPFLAGS = $(AM_CPPFLAGS)
mongoosev_duart_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

endif

if tx39
include_libcpu_HEADERS += tx39/include/tx3904.h
endif

if tx49
include_libcpu_HEADERS += tx49/include/tx4925.h tx49/include/tx4938.h

noinst_PROGRAMS += tx49/timer.rel
tx49_timer_rel_SOURCES = timer/timer.c timer/gettime.S
tx49_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
tx49_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if au1x00
include_libcpu_HEADERS += au1x00/include/au1x00.h

endif

if r46xx
noinst_PROGRAMS += r46xx/timer.rel
r46xx_timer_rel_SOURCES = timer/timer.c timer/gettime.S
r46xx_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
r46xx_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += r46xx/clock.rel
r46xx_clock_rel_SOURCES = clock/ckinit.c clock/clock.S clock/clock.h
r46xx_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
r46xx_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if rm52xx
include_libcpu_HEADERS += rm52xx/include/rm5231.h

noinst_PROGRAMS += rm52xx/timer.rel
rm52xx_timer_rel_SOURCES = timer/timer.c timer/gettime.S
rm52xx_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
rm52xx_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/local.am
