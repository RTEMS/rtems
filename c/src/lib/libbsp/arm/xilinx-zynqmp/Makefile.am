##
#
# @file
#
# @brief Makefile of LibBSP for the Xilinx Zynq UltraScale+ MPSoC platform.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/arm/xilinx-zynqmp/start/bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  Data                                                       #
###############################################################################

start.$(OBJEXT): ../../../../../../bsps/arm/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-exception-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-ttb-entries.c

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/xilinx-zynqmp/start/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/xilinx-zynqmp/start/bspstart.c
if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/arm-a9mpcore-smp.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/xilinx-zynqmp/start/bspsmp.c
endif

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/irq/irq-gic.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/xilinx-zynqmp/console/console-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/serial/zynq-uart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/serial/zynq-uart-polled.c

# Clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/clock-generic-timer.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c

# Start hooks
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/xilinx-zynqmp/start/bspstarthooks.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/xilinx-zynqmp/start/bspstartmmu.c

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/xilinx-zynqmp/headers.am
