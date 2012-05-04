ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h
include_bsp_HEADERS = include/irq.h
include_bsp_HEADERS += include/swi.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

nodist_include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/bspgetworkarea.c ../../shared/bsppretaskinghook.c \
    ../../shared/bsppredriverhook.c ../../shared/bspstart.c \
    ../../shared/bspclean.c startup/bspreset.c ../../shared/bootcard.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c startup/syscalls.c
# console
libbsp_a_SOURCES += ../../shared/console-polled.c console/console-io.c
# clock
libbsp_a_SOURCES += ../../shared/clock_driver_simidle.c
# timer
libbsp_a_SOURCES += ../../shared/timerstub.c
# above
libbsp_a_SOURCES += ../shared/abort/abort.c

#libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/arm920.rel \
#    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/clock.rel \
#    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer.rel \
#    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/irq.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
