#
# @file
#
# @brief Makefile of LibBSP for the generic_or1k BSP.
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

start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =
librtemsbsp_a_CPPFLAGS = $(AM_CPPFLAGS)
librtemsbsp_a_LIBADD =

# Startup
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
librtemsbsp_a_SOURCES += startup/bspstart.c

# Shared
librtemsbsp_a_SOURCES += ../../shared/bootcard.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/src/stackalloc.c

# Console
librtemsbsp_a_SOURCES += ../../shared/console.c
librtemsbsp_a_SOURCES += ../../shared/console_control.c
librtemsbsp_a_SOURCES += ../../shared/console_read.c
librtemsbsp_a_SOURCES += ../../shared/console_select.c
librtemsbsp_a_SOURCES += ../../shared/console_write.c
librtemsbsp_a_SOURCES += console/console-config.c
librtemsbsp_a_SOURCES += console/uart.c

# Timer
librtemsbsp_a_SOURCES += timer/timer.c

# clock
librtemsbsp_a_SOURCES += clock/clockdrv.c ../../../shared/clockdrv_shell.h

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += irq/irq.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/or1k/shared/cache/cache.c

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/or1k/generic_or1k/headers.am
