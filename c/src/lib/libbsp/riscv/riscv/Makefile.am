##
#
# @brief Makefile of LibBSP for the RISCV/Spike simulator.
#
#
ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/riscv/riscv/start/bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  Data                                                       #
###############################################################################

start.$(OBJEXT): ../../../../../../bsps/riscv/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
project_lib_DATA += linkcmds.base

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

# Startup
librtemsbsp_a_SOURCES = ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/riscv/riscv/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/riscv/riscv/start/bsp_fatal_halt.c

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bsp-fdt.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c

# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/riscv/riscv/clock/clockdrv.c

# Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-cpucounter.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/riscv/riscv/irq/irq.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/riscv/riscv/console/console-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/riscv/riscv/console/htif.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/riscv/riscv/console/fe310-uart.c

if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/riscv/riscv/start/bspsmp.c
endif

include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/riscv/riscv/headers.am
