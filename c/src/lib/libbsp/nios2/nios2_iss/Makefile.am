ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

dist_project_lib_DATA = bsp_specs

include_bspdir = $(includedir)/bsp

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
nodist_include_bsp_HEADERS += ../../shared/include/console-polled.h
DISTCLEANFILES = include/bspopts.h

include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = times
noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/crtnn.s
libbspstart_a_SOURCES += start/start.S
project_lib_DATA = start.$(OBJEXT) crtnn.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =
libbsp_a_CPPFLAGS =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspreset_loop.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += startup/setvec.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c

# clock
libbsp_a_SOURCES += clock/clock.c ../../../shared/clockdrv_shell.h
# console
libbsp_a_SOURCES += console/console.c ../../shared/console-polled.c
# timer
libbsp_a_SOURCES += timer/timer.c

# Cache
libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cache.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
