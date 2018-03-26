ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

noinst_PROGRAMS =

if mcf5206
# mcf5206/include
## mcf5206/clock
noinst_PROGRAMS += mcf5206/clock.rel
mcf5206_clock_rel_SOURCES = mcf5206/clock/ckinit.c
mcf5206_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
mcf5206_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## mcf5206/mcfuart
noinst_PROGRAMS += mcf5206/mcfuart.rel
mcf5206_mcfuart_rel_SOURCES = mcf5206/console/mcfuart.c
mcf5206_mcfuart_rel_CPPFLAGS = $(AM_CPPFLAGS)
mcf5206_mcfuart_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## mcf5206/mbus
noinst_PROGRAMS += mcf5206/mbus.rel
mcf5206_mbus_rel_SOURCES = mcf5206/mbus/mcfmbus.c
mcf5206_mbus_rel_CPPFLAGS = $(AM_CPPFLAGS)
mcf5206_mbus_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## mcf5206/timer
noinst_PROGRAMS += mcf5206/timer.rel
mcf5206_timer_rel_SOURCES = mcf5206/timer/timer.c mcf5206/timer/timerisr.S
mcf5206_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
mcf5206_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if mcf5272
## mcf5272/include
## clock
noinst_PROGRAMS += mcf5272/clock.rel
mcf5272_clock_rel_SOURCES = mcf5272/clock/ckinit.c
mcf5272_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
mcf5272_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## timer
noinst_PROGRAMS += mcf5272/timer.rel
mcf5272_timer_rel_SOURCES = mcf5272/timer/timer.c mcf5272/timer/timerisr.S
mcf5272_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
mcf5272_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if mcf548x
## mcf548x/include
## mcf548x/mcdma
noinst_PROGRAMS += mcf548x/mcdma.rel
mcf548x_mcdma_rel_SOURCES = mcf548x/mcdma/MCD_dmaApi.c mcf548x/mcdma/MCD_tasksInit.c \
                            mcf548x/mcdma/MCD_tasks.c  mcf548x/mcdma/mcdma_glue.c

mcf548x_mcdma_rel_CPPFLAGS = $(AM_CPPFLAGS)
mcf548x_mcdma_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(top_srcdir)/../../../automake/local.am
