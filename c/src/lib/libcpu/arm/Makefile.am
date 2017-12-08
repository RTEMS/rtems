ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

EXTRA_DIST =

noinst_PROGRAMS =

include_bspdir = $(includedir)/bsp
include_libcpudir = $(includedir)/libcpu

include_bsp_HEADERS =
include_libcpu_HEADERS =

## shared/include
if shared

include_libcpu_HEADERS += shared/include/mmu.h
include_libcpu_HEADERS += shared/include/arm-cp15.h

## shared/arm920
noinst_PROGRAMS += shared/arm920.rel
shared_arm920_rel_SOURCES = shared/arm920/mmu.c
shared_arm920_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/shared/src
shared_arm920_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if at91rm9200
include_HEADERS = at91rm9200/include/at91rm9200.h at91rm9200/include/at91rm9200_dbgu.h \
    at91rm9200/include/at91rm9200_emac.h at91rm9200/include/at91rm9200_gpio.h \
    at91rm9200/include/at91rm9200_mem.h at91rm9200/include/at91rm9200_pmc.h \
    at91rm9200/include/bits.h at91rm9200/include/at91rm9200_usart.h

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

## at91rm9200/usart
noinst_PROGRAMS += at91rm9200/usart.rel
at91rm9200_usart_rel_SOURCES = at91rm9200/usart/usart.c
at91rm9200_usart_rel_CPPFLAGS = $(AM_CPPFLAGS)
at91rm9200_usart_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

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
include_bsp_HEADERS += at91rm9200/irq/irq.h

noinst_PROGRAMS += at91rm9200/irq.rel
at91rm9200_irq_rel_SOURCES = at91rm9200/irq/irq.c \
    at91rm9200/irq/irq.h
at91rm9200_irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
at91rm9200_irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if s3c24xx
include_HEADERS = s3c24xx/include/s3c24xx.h s3c24xx/include/s3c2400.h s3c24xx/include/s3c2410.h

## s3c24xx/clock
noinst_PROGRAMS += s3c24xx/clock.rel
s3c24xx_clock_rel_SOURCES = s3c24xx/clock/clockdrv.c s3c24xx/clock/support.c
s3c24xx_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
s3c24xx_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## s3c24xx/timer
noinst_PROGRAMS += s3c24xx/timer.rel
s3c24xx_timer_rel_SOURCES = s3c24xx/timer/timer.c
s3c24xx_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
s3c24xx_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## s3c24xx/interrupt
include_bsp_HEADERS += s3c24xx/irq/irq.h

noinst_PROGRAMS += s3c24xx/irq.rel
s3c24xx_irq_rel_SOURCES = s3c24xx/irq/irq.c \
    s3c24xx/irq/irq.h
s3c24xx_irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
s3c24xx_irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/local.am
