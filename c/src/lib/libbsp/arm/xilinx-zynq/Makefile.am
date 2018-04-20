##
#
# @file
#
# @brief Makefile of LibBSP for the Xilinx Zynq platform.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

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
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/cpucounter/cpucounterdiff.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c
librtemsbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
librtemsbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c

# Startup
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += startup/bspstart.c
if HAS_SMP
librtemsbsp_a_SOURCES += ../shared/arm-a9mpcore-smp.c
librtemsbsp_a_SOURCES += startup/bspsmp.c
endif

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../shared/arm-gic-irq.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/xilinx-zynq/console/console-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/xilinx-zynq/console/debug-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/xilinx-zynq/console/zynq-uart.c

# Clock
librtemsbsp_a_SOURCES += ../shared/arm-a9mpcore-clock-config.c

# I2C
librtemsbsp_a_SOURCES += i2c/cadence-i2c.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-l2c-310.c

# Start hooks
librtemsbsp_a_SOURCES += startup/bspstarthooks.c
librtemsbsp_a_SOURCES += startup/bspstartmmu.c

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/xilinx-zynq/headers.am
