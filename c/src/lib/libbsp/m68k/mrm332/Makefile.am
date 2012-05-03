ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

include_HEADERS += include/mrm332.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = times

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds_ROM

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += startup/start_c.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bspstart.c startup/bsppredriverhook.c \
    ../../shared/bspclean.c ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/setvec.c ../../shared/bsppretaskinghook.c \
    ../../shared/bspgetworkarea.c ../../shared/gnatinstallhandler.c \
    startup/except_vect_332_ROM.S
# clock
libbsp_a_SOURCES += clock/ckinit.c
# console
libbsp_a_SOURCES += console/console.c console/sci.c console/sci.h
# spurious
libbsp_a_SOURCES += spurious/spinit.c
# timer
libbsp_a_SOURCES += timer/timer.c

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
