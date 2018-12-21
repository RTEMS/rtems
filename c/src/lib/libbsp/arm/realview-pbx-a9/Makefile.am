##
#
# @file
#
# @brief Makefile of LibBSP for the RealView PBX A9 board.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/arm/realview-pbx-a9/start/bsp_specs

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
dist_project_lib_DATA += ../../../../../../bsps/arm/realview-pbx-a9/start/linkcmds.realview_pbx_a9_qemu

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
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/realview-pbx-a9/start/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/realview-pbx-a9/start/bspstart.c
if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/arm-a9mpcore-smp.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/realview-pbx-a9/start/bspsmp.c
endif

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/irq/irq-gic.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios-init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/getserialmouseps2.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/serial/arm-pl011.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/serial/arm-pl050.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/realview-pbx-a9/console/console-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/realview-pbx-a9/console/console-polled.c

# Clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/clock-a9mpcore.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-v7ar-disable-data.S

# Start hooks
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/realview-pbx-a9/start/bspstarthooks.c

# Framebuffer
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/fb/arm-pl111.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/realview-pbx-a9/start/fb-config.c

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/realview-pbx-a9/headers.am
