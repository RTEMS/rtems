##
#
# @brief Makefile of LibBSP for the Epiphany simulator.
#
#
ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp
include_libcpudir = $(includedir)/libcpu

dist_project_lib_DATA = bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h
include_HEADERS += ../../shared/include/coverhd.h

nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
include_bsp_HEADERS = ../shared/include/linker-symbols.h

include_bsp_HEADERS += ../../../libbsp/shared/include/mm.h
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/stackalloc.h
include_bsp_HEADERS += ../../shared/include/console-polled.h
include_bsp_HEADERS += include/irq.h

nodist_include_HEADERS = include/bspopts.h

###############################################################################
#                  Data                                                       #
###############################################################################
noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = start/start.S

project_lib_DATA = start/start.$(OBJEXT)

project_lib_DATA += startup/linkcmds

###############################################################################
#                  LibBSP                                                     #
###############################################################################

noinst_LIBRARIES += libbsp.a

# Startup
libbsp_a_SOURCES = ../../shared/bspreset.c
libbsp_a_SOURCES += ../../shared/bspstart.c

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
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
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../shared/include/cache_.h
libbsp_a_CPPFLAGS = -I$(srcdir)/../../shared/include

# debugio
libbsp_a_SOURCES += console/console-io.c

DISTCLEANFILES = include/bspopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
