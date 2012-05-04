ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

include_HEADERS += include/coverhd.h

EXTRA_DIST = times

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../shared/bsppretaskinghook.c \
    ../../shared/bsppost.c ../../shared/bspgetworkarea.c \
    ../../shared/bspstart.c startup/init5272.c ../../shared/bootcard.c \
    ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
# console
libbsp_a_SOURCES += console/console-io.c ../../shared/console-polled.c

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5272/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5272/timer.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
