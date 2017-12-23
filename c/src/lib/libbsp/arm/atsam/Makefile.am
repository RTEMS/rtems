ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

###############################################################################
#                  Data                                                       #
###############################################################################

dist_project_lib_DATA = startup/bsp_specs

EXTRA_DIST += ../shared/start/start.S
start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
project_lib_DATA += linkcmds.memory
dist_project_lib_DATA += startup/linkcmds.intsram
dist_project_lib_DATA += startup/linkcmds.sdram
dist_project_lib_DATA += startup/linkcmds.qspiflash

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  LibBSP                                                     #
###############################################################################

noinst_LIBRARIES = libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS = $(AM_CPPFLAGS)
libbsp_a_LIBADD =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c

# libchip
libbsp_a_SOURCES += libraries/libchip/source/acc.c
libbsp_a_SOURCES += libraries/libchip/source/aes.c
libbsp_a_SOURCES += libraries/libchip/source/afec.c
libbsp_a_SOURCES += libraries/libchip/source/afe_dma.c
libbsp_a_SOURCES += libraries/libchip/source/dac_dma.c
libbsp_a_SOURCES += libraries/libchip/source/efc.c
libbsp_a_SOURCES += libraries/libchip/source/exceptions.c
libbsp_a_SOURCES += libraries/libchip/source/flashd.c
libbsp_a_SOURCES += libraries/libchip/source/gmac.c
libbsp_a_SOURCES += libraries/libchip/source/gmacd.c
libbsp_a_SOURCES += libraries/libchip/source/hsmci.c
libbsp_a_SOURCES += libraries/libchip/source/icm.c
libbsp_a_SOURCES += libraries/libchip/source/isi.c
libbsp_a_SOURCES += libraries/libchip/source/mcan.c
libbsp_a_SOURCES += libraries/libchip/source/mediaLB.c
libbsp_a_SOURCES += libraries/libchip/source/mpu.c
libbsp_a_SOURCES += libraries/libchip/source/pio.c
libbsp_a_SOURCES += libraries/libchip/source/pio_capture.c
libbsp_a_SOURCES += libraries/libchip/source/pio_it.c
libbsp_a_SOURCES += libraries/libchip/source/pmc.c
libbsp_a_SOURCES += libraries/libchip/source/pwmc.c
libbsp_a_SOURCES += libraries/libchip/source/qspi.c
libbsp_a_SOURCES += libraries/libchip/source/qspi_dma.c
libbsp_a_SOURCES += libraries/libchip/source/rstc.c
libbsp_a_SOURCES += libraries/libchip/source/rtc.c
libbsp_a_SOURCES += libraries/libchip/source/rtt.c
libbsp_a_SOURCES += libraries/libchip/source/sdramc.c
libbsp_a_SOURCES += libraries/libchip/source/spi.c
libbsp_a_SOURCES += libraries/libchip/source/spi_dma.c
libbsp_a_SOURCES += libraries/libchip/source/ssc.c
libbsp_a_SOURCES += libraries/libchip/source/supc.c
libbsp_a_SOURCES += libraries/libchip/source/tc.c
libbsp_a_SOURCES += libraries/libchip/source/trng.c
libbsp_a_SOURCES += libraries/libchip/source/twi.c
libbsp_a_SOURCES += libraries/libchip/source/twid.c
libbsp_a_SOURCES += libraries/libchip/source/uart.c
libbsp_a_SOURCES += libraries/libchip/source/uart_dma.c
libbsp_a_SOURCES += libraries/libchip/source/usart.c
libbsp_a_SOURCES += libraries/libchip/source/usart_dma.c
libbsp_a_SOURCES += libraries/libchip/source/wdt.c
libbsp_a_SOURCES += libraries/libchip/source/xdmad.c
libbsp_a_SOURCES += libraries/libchip/source/xdma_hardware_interface.c

# libboard
libbsp_a_SOURCES += libraries/libboard/resources_v71/system_samv71.c
libbsp_a_SOURCES += libraries/libboard/source/board_lowlevel.c
libbsp_a_SOURCES += libraries/libboard/source/board_memories.c
libbsp_a_SOURCES += libraries/libboard/source/dbg_console.c

# Startup
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += ../shared/armv7m/startup/bspreset.c
libbsp_a_SOURCES += ../shared/armv7m/startup/armv7m-cpucounter.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/bspstarthooks.c
libbsp_a_SOURCES += startup/getentropy-trng.c
libbsp_a_SOURCES += startup/pin-config.c
libbsp_a_SOURCES += startup/pmc-config.c
libbsp_a_SOURCES += startup/power-clock.c
libbsp_a_SOURCES += startup/power-rtc.c
libbsp_a_SOURCES += startup/power.c
libbsp_a_SOURCES += startup/power-rtc.c
libbsp_a_SOURCES += startup/power-clock.c
libbsp_a_SOURCES += startup/restart.c
libbsp_a_SOURCES += startup/sdram-config.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq.c
libbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq-dispatch.c

# Console
libbsp_a_SOURCES += ../../shared/console-termios.c
libbsp_a_SOURCES += console/console.c
libbsp_a_SOURCES += console/debug-console.c

# Clock
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
libbsp_a_SOURCES += ../shared/armv7m/clock/armv7m-clock-config.c
libbsp_a_SOURCES += clock/systick-freq.c

# Timer
libbsp_a_SOURCES += ../../shared/timerstub.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../shared/armv7m/include/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../shared/armv7m/include

# Network
if HAS_NETWORKING
libbsp_a_SOURCES += network/if_atsam.c
endif

# I2C
libbsp_a_SOURCES += i2c/atsam_i2c_bus.c
libbsp_a_SOURCES += i2c/atsam_i2c_init.c

# SPI
libbsp_a_SOURCES += spi/atsam_spi_bus.c
libbsp_a_SOURCES += spi/atsam_spi_init.c
libbsp_a_SOURCES += spi/sc16is752.c

# RTC
libbsp_a_SOURCES += ../../shared/tod.c
libbsp_a_SOURCES += rtc/rtc-config.c

# Includes
libbsp_a_CPPFLAGS += -I$(srcdir)/../shared/CMSIS/Include
libbsp_a_CPPFLAGS += -I$(srcdir)/libraries/libboard
libbsp_a_CPPFLAGS += -I$(srcdir)/libraries/libboard/include
libbsp_a_CPPFLAGS += -I$(srcdir)/libraries/libchip
libbsp_a_CPPFLAGS += -I$(srcdir)/libraries/libchip/include

# hack to get the BSP to build
libbsp_a_CPPFLAGS += -I$(RTEMS_SOURCE_ROOT)/bsps/arm/atsam/include/libchip
libbsp_a_CPPFLAGS += -I$(RTEMS_SOURCE_ROOT)/bsps/arm/atsam/include/libchip/include

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/atsam/headers.am
