#
# @file
#
# @brief Makefile of LibBSP for the generic_or1k BSP.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/or1k/generic_or1k/start/bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  Data                                                       #
###############################################################################

start.$(OBJEXT): ../../../../../../bsps/or1k/generic_or1k/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/or1k/shared/start/linkcmds.base

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/or1k/generic_or1k/start/bspstart.c

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/or1k/generic_or1k/console/console-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/or1k/generic_or1k/console/uart.c

# Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/or1k/generic_or1k/btimer/btimer.c

# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/or1k/generic_or1k/clock/clockdrv.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/or1k/generic_or1k/irq/irq.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/or1k/shared/cache/cache.c

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/or1k/generic_or1k/headers.am
