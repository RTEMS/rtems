##
#
# @file
#
# @brief Makefile of LibBSP for the Altera Cyclone-V platform.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp
include_bsp_socaldir = $(includedir)/bsp/socal
include_libcpudir = $(includedir)/libcpu

dist_project_lib_DATA = bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = ../../shared/include/coverhd.h \
	include/bspopts.h

nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h

include_bsp_HEADERS =
include_bsp_socal_HEADERS =

include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/stackalloc.h
include_bsp_HEADERS += ../shared/include/start.h
include_bsp_HEADERS += ../shared/include/arm-a9mpcore-clock.h
include_bsp_HEADERS += ../shared/include/arm-a9mpcore-irq.h
include_bsp_HEADERS += ../shared/include/arm-a9mpcore-regs.h
include_bsp_HEADERS += ../shared/include/arm-a9mpcore-start.h
include_bsp_HEADERS += ../shared/include/arm-cp15-start.h
include_bsp_HEADERS += ../shared/include/arm-errata.h
include_bsp_HEADERS += ../shared/include/arm-gic.h
include_bsp_HEADERS += ../shared/include/arm-gic-irq.h
include_bsp_HEADERS += ../shared/include/arm-gic-regs.h
include_bsp_HEADERS += ../shared/include/arm-gic-tm27.h
include_bsp_HEADERS += ../shared/include/arm-release-id.h
include_bsp_HEADERS += include/irq.h
include_bsp_HEADERS += include/nocache-heap.h

# Altera hwlib
include_bsp_HEADERS += hwlib/include/alt_16550_uart.h
include_bsp_HEADERS += hwlib/include/alt_address_space.h
include_bsp_HEADERS += hwlib/include/alt_cache.h
include_bsp_HEADERS += hwlib/include/alt_clock_group.h
include_bsp_HEADERS += hwlib/include/alt_clock_manager.h
include_bsp_HEADERS += hwlib/include/alt_dma_common.h
include_bsp_HEADERS += hwlib/include/alt_dma.h
include_bsp_HEADERS += hwlib/include/alt_dma_program.h
include_bsp_HEADERS += hwlib/include/alt_generalpurpose_io.h
include_bsp_HEADERS += hwlib/include/alt_hwlibs_ver.h
include_bsp_HEADERS += hwlib/include/alt_i2c.h
include_bsp_HEADERS += hwlib/include/alt_interrupt_common.h
include_bsp_HEADERS += hwlib/include/alt_mpu_registers.h
include_bsp_HEADERS += hwlib/include/alt_qspi_private.h
include_bsp_HEADERS += hwlib/include/alt_reset_manager.h
include_bsp_HEADERS += hwlib/include/hwlib.h
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
include_bsp_socal_HEADERS += hwlib/include/socal/alt_acpidmap.h
include_bsp_socal_HEADERS += hwlib/include/socal/alt_clkmgr.h
include_bsp_socal_HEADERS += hwlib/include/socal/alt_dmanonsecure.h
include_bsp_socal_HEADERS += hwlib/include/socal/alt_dmasecure.h
include_bsp_socal_HEADERS += hwlib/include/socal/alt_gpio.h
include_bsp_socal_HEADERS += hwlib/include/socal/alt_i2c.h
include_bsp_socal_HEADERS += hwlib/include/socal/alt_l3.h
include_bsp_socal_HEADERS += hwlib/include/socal/alt_qspidata.h
include_bsp_socal_HEADERS += hwlib/include/socal/alt_qspi.h
include_bsp_socal_HEADERS += hwlib/include/socal/alt_rstmgr.h
include_bsp_socal_HEADERS += hwlib/include/socal/alt_sdr.h
include_bsp_socal_HEADERS += hwlib/include/socal/alt_sysmgr.h
include_bsp_socal_HEADERS += hwlib/include/socal/alt_uart.h
include_bsp_socal_HEADERS += hwlib/include/socal/hps.h
include_bsp_socal_HEADERS += hwlib/include/socal/socal.h

include_libcpu_HEADERS = ../../../libcpu/arm/shared/include/arm-cp15.h



###############################################################################
#                  Data                                                       #
###############################################################################

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = ../shared/start/start.S

project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += startup/linkcmds
project_lib_DATA += startup/linkcmds.altcycv
project_lib_DATA += startup/linkcmds.altcycv_devkit
project_lib_DATA += startup/linkcmds.altcycv_devkit_smp

###############################################################################
#                  LibBSP                                                     #
###############################################################################

noinst_LIBRARIES += libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS =
libbsp_a_LIBADD =

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
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsplibc.c
libbsp_a_SOURCES += ../../shared/bsppost.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/bsppretaskinghook.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/timerstub.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
libbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c

# Startup
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/bspstarthooks.c
libbsp_a_SOURCES += startup/nocache-heap.c
libbsp_a_SOURCES += startup/mmu-config.c
if HAS_SMP
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

# Network
libbsp_a_SOURCES += network/network.c

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
include_bsp_HEADERS += include/i2cdrv.h

# RTC
libbsp_a_SOURCES += ../../shared/tod.c
libbsp_a_SOURCES += rtc/rtc.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../shared/include/arm-cache-l1.h
libbsp_a_SOURCES += ../shared/arm-l2c-310/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../shared/arm-l2c-310

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
