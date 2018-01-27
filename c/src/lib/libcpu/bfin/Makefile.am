ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

EXTRA_DIST =

noinst_PROGRAMS =


############
# Start of bf52x files
if bf52x

## INTERRUPT
noinst_PROGRAMS += bf52x/interrupt.rel
bf52x_interrupt_rel_SOURCES = bf52x/interrupt/interrupt.c \
                              bf52x/interrupt/interrupt.h
bf52x_interrupt_rel_CPPFLAGS = $(AM_CPPFLAGS)
bf52x_interrupt_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

endif
# endof bf52x
############

noinst_PROGRAMS += mmu.rel
mmu_rel_SOURCES = mmu/mmu.c
mmu_rel_CPPFLAGS = $(AM_CPPFLAGS)
mmu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if bf52x

else
noinst_PROGRAMS += interrupt.rel
interrupt_rel_SOURCES = interrupt/interrupt.c
interrupt_rel_CPPFLAGS = $(AM_CPPFLAGS)
interrupt_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

endif

noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/clock.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += rtc.rel
rtc_rel_SOURCES = clock/rtc.c clock/rtc.h
rtc_rel_CPPFLAGS = $(AM_CPPFLAGS)
rtc_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += uart.rel
uart_rel_SOURCES = serial/uart.c
uart_rel_CPPFLAGS = $(AM_CPPFLAGS)
uart_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += sport.rel
sport_rel_SOURCES = serial/sport.c
sport_rel_CPPFLAGS = $(AM_CPPFLAGS)
sport_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += spi.rel
spi_rel_SOURCES = serial/spi.c
spi_rel_CPPFLAGS = $(AM_CPPFLAGS)
spi_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += twi.rel
twi_rel_SOURCES = serial/twi.c
twi_rel_CPPFLAGS = $(AM_CPPFLAGS)
twi_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_NETWORKING
## network
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/ethernet.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(top_srcdir)/../../../automake/local.am
