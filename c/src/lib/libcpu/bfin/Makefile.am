ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

EXTRA_DIST =

noinst_PROGRAMS =

include_bspdir = $(includedir)/bsp
include_libcpudir = $(includedir)/libcpu

include_bsp_HEADERS =
include_libcpu_HEADERS =


############
# Start of bf52x files
if bf52x

include_HEADERS = bf52x/include/bf52x.h

## INTERRUPT
include_bsp_HEADERS += bf52x/interrupt/interrupt.h
noinst_PROGRAMS += bf52x/interrupt.rel
bf52x_interrupt_rel_SOURCES = bf52x/interrupt/interrupt.c \
                              bf52x/interrupt/interrupt.h
bf52x_interrupt_rel_CPPFLAGS = $(AM_CPPFLAGS)
bf52x_interrupt_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

endif
# endof bf52x
############

include_libcpu_HEADERS += include/bf533.h
include_libcpu_HEADERS += include/bf537.h
include_libcpu_HEADERS += include/cecRegs.h
include_libcpu_HEADERS += include/memoryRegs.h
include_libcpu_HEADERS += include/mmuRegs.h
include_libcpu_HEADERS += include/sicRegs.h
include_libcpu_HEADERS += include/ebiuRegs.h
include_libcpu_HEADERS += include/ppiRegs.h
include_libcpu_HEADERS += include/coreTimerRegs.h
include_libcpu_HEADERS += include/wdogRegs.h
include_libcpu_HEADERS += include/timerRegs.h
include_libcpu_HEADERS += include/dmaRegs.h
include_libcpu_HEADERS += include/ethernetRegs.h
include_libcpu_HEADERS += include/uartRegs.h
include_libcpu_HEADERS += include/sportRegs.h
include_libcpu_HEADERS += include/twiRegs.h
include_libcpu_HEADERS += include/spiRegs.h
include_libcpu_HEADERS += include/rtcRegs.h
include_libcpu_HEADERS += include/gpioRegs.h


include_libcpu_HEADERS += ../shared/include/cache.h
noinst_PROGRAMS += cache.rel
cache_rel_SOURCES = cache/cache.c ../shared/src/cache_aligned_malloc.c \
    ../shared/src/cache_manager.c cache/cache_.h
cache_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/cache
cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_libcpu_HEADERS += mmu/mmu.h
noinst_PROGRAMS += mmu.rel
mmu_rel_SOURCES = mmu/mmu.c
mmu_rel_CPPFLAGS = $(AM_CPPFLAGS)
mmu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if bf52x

else
include_libcpu_HEADERS += interrupt/interrupt.h
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
rtc_rel_SOURCES = clock/rtc.c
rtc_rel_CPPFLAGS = $(AM_CPPFLAGS)
rtc_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_libcpu_HEADERS += serial/uart.h
noinst_PROGRAMS += uart.rel
uart_rel_SOURCES = serial/uart.c
uart_rel_CPPFLAGS = $(AM_CPPFLAGS)
uart_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_libcpu_HEADERS += serial/sport.h
noinst_PROGRAMS += sport.rel
sport_rel_SOURCES = serial/sport.c
sport_rel_CPPFLAGS = $(AM_CPPFLAGS)
sport_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_libcpu_HEADERS += serial/spi.h
noinst_PROGRAMS += spi.rel
spi_rel_SOURCES = serial/spi.c
spi_rel_CPPFLAGS = $(AM_CPPFLAGS)
spi_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_libcpu_HEADERS += serial/twi.h
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
include_libcpu_HEADERS += network/ethernet.h
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/ethernet.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/local.am
