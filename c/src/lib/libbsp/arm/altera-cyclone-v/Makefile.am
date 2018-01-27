##
#
# @file
#
# @brief Makefile of LibBSP for the Altera Cyclone-V platform.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

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

EXTRA_DIST += ../shared/start/start.S
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

noinst_LIBRARIES = libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS = $(AM_CPPFLAGS)

# for the Altera hwlib
libbsp_a_CPPFLAGS += -I${srcdir}/hwlib/include
libbsp_a_CPPFLAGS += -std=gnu99
CFLAGS += -Wno-missing-prototypes

# hwlib from Altera
libbsp_a_SOURCES += hwlib/src/hwmgr/alt_16550_uart.c
libbsp_a_SOURCES += hwlib/src/hwmgr/alt_address_space.c
libbsp_a_SOURCES += hwlib/src/hwmgr/alt_clock_manager.c
libbsp_a_SOURCES += hwlib/src/hwmgr/alt_dma.c
libbsp_a_SOURCES += hwlib/src/hwmgr/alt_dma_program.c
libbsp_a_SOURCES += hwlib/src/hwmgr/alt_generalpurpose_io.c
libbsp_a_SOURCES += hwlib/src/hwmgr/alt_i2c.c
libbsp_a_SOURCES += hwlib/src/hwmgr/alt_qspi.c
libbsp_a_SOURCES += hwlib/src/hwmgr/alt_reset_manager.c
#The following Altera hwlib source files have been left out because so far 
#they are not required:
#libbsp_a_SOURCES += hwlib/src/hwmgr/alt_bridge_manager.c
#libbsp_a_SOURCES += hwlib/src/hwmgr/alt_fpga_manager.c
#libbsp_a_SOURCES += hwlib/src/hwmgr/alt_globaltmr.c
#libbsp_a_SOURCES += hwlib/src/hwmgr/alt_system_manager.c
#libbsp_a_SOURCES += hwlib/src/hwmgr/alt_timers.c
#libbsp_a_SOURCES += hwlib/src/hwmgr/alt_watchdog.c
# The following Altera hwlib source files would be problematic with RTEMS:
#libbsp_a_SOURCES += hwlib/src/hwmgr/alt_interrupt.c


# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/timerstub.c
libbsp_a_SOURCES += ../../shared/src/bsp-fdt.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
libbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c

# Startup
libbsp_a_SOURCES += startup/bspclean.c
libbsp_a_SOURCES += startup/bspgetworkarea.c
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/bspstarthooks.c
libbsp_a_SOURCES += startup/mmu-config.c
if HAS_SMP
libbsp_a_SOURCES += ../shared/arm-a9mpcore-smp.c
libbsp_a_SOURCES += startup/bspsmp.c
endif

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += ../shared/arm-gic-irq.c

# Console
libbsp_a_SOURCES += ../../shared/console-termios-init.c
libbsp_a_SOURCES += ../../shared/console-termios.c
libbsp_a_SOURCES += console/console-config.c

# Clock
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
libbsp_a_SOURCES += ../shared/arm-a9mpcore-clock-config.c

# I2C
libbsp_a_SOURCES += i2c/i2cdrv.c
libbsp_a_SOURCES += i2c/i2cdrv-config.c
# RTC
libbsp_a_SOURCES += ../../shared/tod.c
libbsp_a_SOURCES += rtc/rtc.c

# Cache
libbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-l2c-310.c

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/altera-cyclone-v/headers.am
