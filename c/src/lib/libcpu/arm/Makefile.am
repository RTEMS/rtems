##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

EXTRA_DIST =

noinst_PROGRAMS =

## shared/include
if shared
include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = shared/include/mmu.h

## shared/arm920
noinst_PROGRAMS += shared/arm920.rel
shared_arm920_rel_SOURCES = shared/arm920/mmu.c
shared_arm920_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/shared/src
shared_arm920_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if at91rm9200
include_HEADERS = at91rm9200/include/at91rm9200.h at91rm9200/include/at91rm9200_dbgu.h \
    at91rm9200/include/at91rm9200_emac.h at91rm9200/include/at91rm9200_gpio.h \
    at91rm9200/include/at91rm9200_mem.h at91rm9200/include/at91rm9200_pmc.h at91rm9200/include/bits.h

## at91rm9200/clock
noinst_PROGRAMS += at91rm9200/clock.rel
at91rm9200_clock_rel_SOURCES = at91rm9200/clock/clock.c
at91rm9200_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
at91rm9200_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## at91rm9200/dbgu
noinst_PROGRAMS += at91rm9200/dbgu.rel
at91rm9200_dbgu_rel_SOURCES = at91rm9200/dbgu/dbgu.c
at91rm9200_dbgu_rel_CPPFLAGS = $(AM_CPPFLAGS)
at91rm9200_dbgu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## at91rm9200/timer
noinst_PROGRAMS += at91rm9200/timer.rel
at91rm9200_timer_rel_SOURCES = at91rm9200/timer/timer.c
at91rm9200_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
at91rm9200_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## at91rm9200/pmc
noinst_PROGRAMS += at91rm9200/pmc.rel
at91rm9200_pmc_rel_SOURCES = at91rm9200/pmc/pmc.c
at91rm9200_pmc_rel_CPPFLAGS = $(AM_CPPFLAGS)
at91rm9200_pmc_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## at91rm9200/interrupt
include_HEADERS += at91rm9200/irq/irq.h

noinst_PROGRAMS += at91rm9200/irq.rel
at91rm9200_irq_rel_SOURCES = at91rm9200/irq/irq.c at91rm9200/irq/bsp_irq_init.c \
    ../../libbsp/arm/shared/irq/irq_init.c at91rm9200/irq/bsp_irq_asm.S \
    ../../libbsp/arm/shared/irq/irq_asm.S at91rm9200/irq/irq.h
at91rm9200_irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
at91rm9200_irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if mc9328mxl
include_HEADERS = mc9328mxl/include/mc9328mxl.h

## mc9328mxl/clock
noinst_PROGRAMS += mc9328mxl/clock.rel
mc9328mxl_clock_rel_SOURCES = mc9328mxl/clock/clockdrv.c
mc9328mxl_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
mc9328mxl_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## mc9328mxl/timer
noinst_PROGRAMS += mc9328mxl/timer.rel
mc9328mxl_timer_rel_SOURCES = mc9328mxl/timer/timer.c
mc9328mxl_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
mc9328mxl_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## mc9328mxl/interrupt
include_HEADERS += mc9328mxl/irq/irq.h

noinst_PROGRAMS += mc9328mxl/irq.rel
mc9328mxl_irq_rel_SOURCES = mc9328mxl/irq/irq.c mc9328mxl/irq/bsp_irq_init.c \
    ../../libbsp/arm/shared/irq/irq_init.c mc9328mxl/irq/bsp_irq_asm.S \
    ../../libbsp/arm/shared/irq/irq_asm.S mc9328mxl/irq/irq.h
mc9328mxl_irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
mc9328mxl_irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if s3c2400
include_HEADERS = s3c24xx/include/s3c24xx.h s3c2400/include/s3c2400.h

## s3c2400/clock
noinst_PROGRAMS += s3c2400/clock.rel
s3c2400_clock_rel_SOURCES = s3c2400/clock/clockdrv.c s3c2400/clock/support.c
s3c2400_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
s3c2400_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## s3c2400/timer
noinst_PROGRAMS += s3c2400/timer.rel
s3c2400_timer_rel_SOURCES = s3c2400/timer/timer.c
s3c2400_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
s3c2400_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## s3c2400/interrupt
include_HEADERS += s3c2400/irq/irq.h

noinst_PROGRAMS += s3c2400/irq.rel
s3c2400_irq_rel_SOURCES = s3c24xx/irq/irq.c s3c24xx/irq/bsp_irq_init.c \
    ../../libbsp/arm/shared/irq/irq_init.c s3c2400/irq/bsp_irq_asm.S \
    ../../libbsp/arm/shared/irq/irq_asm.S s3c2400/irq/irq.h
s3c2400_irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
s3c2400_irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if s3c2410
include_HEADERS = s3c24xx/include/s3c24xx.h s3c2410/include/s3c2410.h

## s3c2410/clock
noinst_PROGRAMS += s3c2410/clock.rel
s3c2410_clock_rel_SOURCES = s3c24xx/clock/clockdrv.c s3c24xx/clock/support.c
s3c2410_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
s3c2410_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## s3c2410/timer
noinst_PROGRAMS += s3c2410/timer.rel
s3c2410_timer_rel_SOURCES = s3c24xx/timer/timer.c
s3c2410_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
s3c2410_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## s3c2410/interrupt
include_HEADERS += s3c2410/irq/irq.h

noinst_PROGRAMS += s3c2410/irq.rel
s3c2410_irq_rel_SOURCES = s3c24xx/irq/irq.c s3c24xx/irq/bsp_irq_init.c \
    ../../libbsp/arm/shared/irq/irq_init.c s3c2410/irq/bsp_irq_asm.S \
    ../../libbsp/arm/shared/irq/irq_asm.S s3c2410/irq/irq.h
s3c2410_irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
s3c2410_irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if lpc22xx
include_HEADERS = lpc22xx/include/lpc22xx.h

## lpc22xx/clock
noinst_PROGRAMS += lpc22xx/clock.rel
lpc22xx_clock_rel_SOURCES = lpc22xx/clock/clockdrv.c 
lpc22xx_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
lpc22xx_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## lpc22xx/timer
noinst_PROGRAMS += lpc22xx/timer.rel
lpc22xx_timer_rel_SOURCES = lpc22xx/timer/timer.c
lpc22xx_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
lpc22xx_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## lpc22xx/interrupt
include_HEADERS += lpc22xx/irq/irq.h

noinst_PROGRAMS += lpc22xx/irq.rel
lpc22xx_irq_rel_SOURCES = lpc22xx/irq/irq.c lpc22xx/irq/bsp_irq_init.c \
    ../../libbsp/arm/shared/irq/irq_init.c lpc22xx/irq/bsp_irq_asm.S \
    ../../libbsp/arm/shared/irq/irq_asm.S lpc22xx/irq/irq.h
lpc22xx_irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
lpc22xx_irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/local.am
