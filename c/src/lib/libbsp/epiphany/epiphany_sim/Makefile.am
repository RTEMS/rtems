##
#
# @brief Makefile of LibBSP for the Epiphany simulator.
#
#
ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  Data                                                       #
###############################################################################

EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

###############################################################################
#                  LibBSP                                                     #
###############################################################################

noinst_LIBRARIES = libbsp.a

# Startup
libbsp_a_SOURCES = ../../shared/bspreset.c
libbsp_a_SOURCES += ../../shared/bspstart.c

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c

# clock
libbsp_a_SOURCES += ../../shared/clock_driver_simidle.c

# Timer
libbsp_a_SOURCES += timer/timer.c

# console
libbsp_a_SOURCES += ../../shared/console-polled.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += irq/irq.c

# Cache
libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

# debugio
libbsp_a_SOURCES += console/console-io.c

DISTCLEANFILES = include/bspopts.h

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/epiphany/epiphany_sim/headers.am
