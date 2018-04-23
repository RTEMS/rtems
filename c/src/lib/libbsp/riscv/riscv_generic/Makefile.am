##
#
# @brief Makefile of LibBSP for the RISCV/Spike simulator.
#
#
ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/riscv/riscv_generic/start/bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  Data                                                       #
###############################################################################

start.$(OBJEXT): ../../../../../../bsps/riscv/riscv_generic/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

# Startup
librtemsbsp_a_SOURCES = ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspstart-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/riscv/riscv_generic/start/bsp_fatal_halt.c

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c

# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/riscv/riscv_generic/clock/clockdrv.c

# Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/riscv/riscv_generic/btimer/btimer.c

# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-polled.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/riscv/riscv_generic/irq/irq.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

# debugio
librtemsbsp_a_SOURCES += ../../../../../../bsps/riscv/riscv_generic/console/console-io.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/riscv/riscv_generic/headers.am
