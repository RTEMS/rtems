##
#
# @file
#
# @brief Makefile of LibBSP for the Altera Cyclone-V platform.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

# Altera hwlib
#The following Altera hwlib header files have been left out because so far 
#they are not required:
#include_bsp_HEADERS += hwlib/include/alt_bridge_manager.h
#include_bsp_HEADERS += hwlib/include/alt_fpga_manager.h
#include_bsp_HEADERS += hwlib/include/alt_globaltmr.h
#include_bsp_HEADERS += hwlib/include/alt_system_manager.h
#include_bsp_HEADERS += hwlib/include/alt_timers.h
#include_bsp_HEADERS += hwlib/include/alt_watchdog.h
#The following Altera hwlib headers would be problematic with RTEMS:
#include_bsp_HEADERS += hwlib/include/alt_interrupt.h

# Some of the headers from hwlib need the files from socal. Install them.
###############################################################################
#                  Data                                                       #
###############################################################################

start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.altcycv
dist_project_lib_DATA += startup/linkcmds.altcycv_devkit
dist_project_lib_DATA += startup/linkcmds.altcycv_devkit_smp

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =
librtemsbsp_a_CPPFLAGS = $(AM_CPPFLAGS)

# for the Altera hwlib
librtemsbsp_a_CPPFLAGS += -I${srcdir}/hwlib/include
librtemsbsp_a_CPPFLAGS += -std=gnu99
CFLAGS += -Wno-missing-prototypes

# hwlib from Altera
librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_16550_uart.c
librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_address_space.c
librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_clock_manager.c
librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_dma.c
librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_dma_program.c
librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_generalpurpose_io.c
librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_i2c.c
librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_qspi.c
librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_reset_manager.c
#The following Altera hwlib source files have been left out because so far 
#they are not required:
#librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_bridge_manager.c
#librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_fpga_manager.c
#librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_globaltmr.c
#librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_system_manager.c
#librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_timers.c
#librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_watchdog.c
# The following Altera hwlib source files would be problematic with RTEMS:
#librtemsbsp_a_SOURCES += hwlib/src/hwmgr/alt_interrupt.c


# Shared
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterdiff.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/timerstub.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bsp-fdt.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c
librtemsbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
librtemsbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c

# Startup
librtemsbsp_a_SOURCES += startup/bspclean.c
librtemsbsp_a_SOURCES += startup/bspgetworkarea.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/bspstarthooks.c
librtemsbsp_a_SOURCES += startup/mmu-config.c
if HAS_SMP
librtemsbsp_a_SOURCES += ../shared/arm-a9mpcore-smp.c
librtemsbsp_a_SOURCES += startup/bspsmp.c
endif

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../shared/arm-gic-irq.c

# Console
librtemsbsp_a_SOURCES += ../../shared/console-termios-init.c
librtemsbsp_a_SOURCES += ../../shared/console-termios.c
librtemsbsp_a_SOURCES += console/console-config.c

# Clock
librtemsbsp_a_SOURCES += ../shared/arm-a9mpcore-clock-config.c

# I2C
librtemsbsp_a_SOURCES += i2c/i2cdrv.c
librtemsbsp_a_SOURCES += i2c/i2cdrv-config.c
# RTC
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += rtc/rtc.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-l2c-310.c

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/altera-cyclone-v/headers.am
