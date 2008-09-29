##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

include_HEADERS += include/ep7312.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

include_HEADERS += ../../arm/shared/comm/uart.h
# startup
libbsp_a_SOURCES += ../../shared/bsppost.c ../../shared/bsplibc.c \
    ../../shared/bspgetworkarea.c ../../shared/bsppretaskinghook.c \
    ../../shared/bsppredriverhook.c startup/bspstart.c \
    ../../shared/bspclean.c ../../shared/bootcard.c ../../shared/sbrk.c \
    startup/bspreset.c ../../shared/gnatinstallhandler.c

# clock
libbsp_a_SOURCES += clock/clockdrv.c
# console
libbsp_a_SOURCES += console/uart.c ../../shared/console.c
# timer
libbsp_a_SOURCES += timer/timer.c
# abort
libbsp_a_SOURCES += ../shared/abort/abort.c

include_HEADERS += irq/irq.h
# irq
libbsp_a_SOURCES += irq/irq.c irq/bsp_irq_init.c \
    ../../arm/shared/irq/irq_init.c irq/bsp_irq_asm.S \
    ../../arm/shared/irq/irq_asm.S irq/irq.h

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

# libcpu libraries could go here
libbsp_a_LIBADD = 
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
