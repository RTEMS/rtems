##
#
# @file
#
# @brief Makefile of LibBSP for the RealView PBX A9 board.
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

start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.realview_pbx_a9_qemu
dist_project_lib_DATA += startup/linkcmds.realview_pbx_a9_qemu_smp

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterdiff.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
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
librtemsbsp_a_SOURCES += ../../shared/console-termios-init.c
librtemsbsp_a_SOURCES += ../../shared/console-termios.c
librtemsbsp_a_SOURCES += ../../shared/get-serial-mouse-ps2.c
librtemsbsp_a_SOURCES += ../shared/arm-pl011.c
librtemsbsp_a_SOURCES += ../shared/arm-pl050.c
librtemsbsp_a_SOURCES += console/console-config.c

# Clock
librtemsbsp_a_SOURCES += ../shared/arm-a9mpcore-clock-config.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c

# Start hooks
librtemsbsp_a_SOURCES += startup/bspstarthooks.c

# Framebuffer
librtemsbsp_a_SOURCES += ../shared/arm-pl111-fb.c
librtemsbsp_a_SOURCES += startup/fb-config.c

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/realview-pbx-a9/headers.am
