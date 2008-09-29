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

include_HEADERS += include/coverhd.h

EXTRA_DIST = ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bspreset_fatal.c ../../shared/bsppost.c \
    startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/bsppredriverhook.c ../../shared/bspgetworkarea.c \
    ../../shared/bsppretaskinghook.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c

# dlentry
libbsp_a_SOURCES += dlentry/dlentry.S

include_bsp_HEADERS = ../../powerpc/shared/console/uart.h \
    ../../powerpc/shared/console/consoleIo.h
# console
libbsp_a_SOURCES += ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c

include_bsp_HEADERS += irq/irq.h \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/vectors.h \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/ppc_exc_bspsupp.h

# irq
libbsp_a_SOURCES += irq/irq_init.c irq/irq.c

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/timer.rel 

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
