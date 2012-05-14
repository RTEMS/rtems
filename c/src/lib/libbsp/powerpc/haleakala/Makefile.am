ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

include_HEADERS += include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = ../../powerpc/shared/start/rtems_crti.S
project_lib_DATA = rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
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
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h

# irq
libbsp_a_SOURCES += irq/irq_init.c irq/irq.c

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/timer.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
