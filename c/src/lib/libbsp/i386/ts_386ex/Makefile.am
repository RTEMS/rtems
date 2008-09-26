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

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools

include_HEADERS += include/coverhd.h

include_bsp_HEADERS = ../../i386/shared/irq/irq.h \
    ../../i386/shared/irq/irq_asm.h

noinst_HEADERS = start/80386ex.h start/80386ex.inc start/macros.inc \
    start/ts_1325.inc
EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

include_HEADERS += ../../i386/shared/comm/uart.h
startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../shared/bsppretaskinghook.c \
    ../../shared/bspgetworkarea.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/sbrk.c ../../i386/shared/irq/idt.c \
    ../../i386/shared/irq/irq.c ../../i386/shared/irq/irq_init.c \
    ../../i386/shared/comm/i386-stub.c \
    ../../i386/shared/comm/i386-stub-glue.c ../../i386/shared/comm/uart.c \
    ../../shared/gnatinstallhandler.c ../../i386/shared/comm/gdb_glue.c \
    ../../i386/shared/comm/tty_drv.c ../../i386/shared/irq/irq_asm.S
clock_SOURCES = clock/ckinit.c clock/rtc.c
console_SOURCES = console/console.c
include_HEADERS += ../../i386/shared/pci/pcibios.h
pci_SOURCES = ../../i386/shared/pci/pcibios.c \
    ../../i386/shared/pci/pcibios.h
timer_SOURCES = timer/timer.c timer/timerisr.S

if HAS_NETWORKING
include_HEADERS += include/wd80x3.h

# dec21140 is supported via libchip
# FIXME: Do we really need these flags?
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/ne2000.c network/wd80x3.h
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(pci_SOURCES) $(timer_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/score.rel
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
