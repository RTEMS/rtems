##
## $Id$
##

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
    shared/interrupts/vectorexceptions.c shared/interrupts/interruptmask.c \
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

## mongoosev/vectorisrs

noinst_PROGRAMS += mongoosev/vectorisrs.rel
mongoosev_vectorisrs_rel_SOURCES = mongoosev/vectorisrs/maxvectors.c \
   mongoosev/vectorisrs/vectorisrs.c
mongoosev_vectorisrs_rel_CPPFLAGS = $(AM_CPPFLAGS)
mongoosev_vectorisrs_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if tx39
include_libcpu_HEADERS += tx39/include/tx3904.h

noinst_PROGRAMS += tx39/vectorisrs.rel
tx39_vectorisrs_rel_SOURCES = tx39/vectorisrs/maxvectors.c tx39/vectorisrs/vectorisrs.c
tx39_vectorisrs_rel_CPPFLAGS = $(AM_CPPFLAGS)
tx39_vectorisrs_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if tx49
include_libcpu_HEADERS += tx49/include/tx4925.h tx49/include/tx4938.h

noinst_PROGRAMS += tx49/vectorisrs.rel
tx49_vectorisrs_rel_SOURCES = tx49/vectorisrs/maxvectors.c tx49/vectorisrs/vectorisrs.c
tx49_vectorisrs_rel_CPPFLAGS = $(AM_CPPFLAGS)
tx49_vectorisrs_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += tx49/timer.rel
tx49_timer_rel_SOURCES = timer/timer.c timer/gettime.S
tx49_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
tx49_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if au1x00
include_libcpu_HEADERS += au1x00/include/au1x00.h

noinst_PROGRAMS += au1x00/vectorisrs.rel
au1x00_vectorisrs_rel_SOURCES = au1x00/vectorisrs/maxvectors.c \
  au1x00/vectorisrs/vectorisrs.c
au1x00_vectorisrs_rel_CPPFLAGS = $(AM_CPPFLAGS)
au1x00_vectorisrs_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if r46xx
noinst_PROGRAMS += r46xx/vectorisrs.rel
r46xx_vectorisrs_rel_SOURCES = r46xx/vectorisrs/maxvectors.c \
   r46xx/vectorisrs/vectorisrs.c
r46xx_vectorisrs_rel_CPPFLAGS = $(AM_CPPFLAGS)
r46xx_vectorisrs_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

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

noinst_PROGRAMS += rm52xx/vectorisrs.rel
rm52xx_vectorisrs_rel_SOURCES = rm52xx/vectorisrs/maxvectors.c rm52xx/vectorisrs/vectorisrs.c
rm52xx_vectorisrs_rel_CPPFLAGS = $(AM_CPPFLAGS)
rm52xx_vectorisrs_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += rm52xx/timer.rel
rm52xx_timer_rel_SOURCES = timer/timer.c timer/gettime.S
rm52xx_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
rm52xx_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/local.am
