##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h
include_HEADERS += include/usc.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

nodist_include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsplibc.c ../../shared/bsppost.c \
   ../../shared/bsppretaskinghook.c ../../shared/bspgetworkarea.c \
    ../../shared/bsppredriverhook.c startup/bspstart.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c ../../shared/setvec.c \
    startup/inittlb.c \
    startup/idtmem.S startup/idttlb.S startup/exception.S
# clock
libbsp_a_SOURCES += clock/ckinit.c
# console
libbsp_a_SOURCES += console/console.c
# liblnk
libbsp_a_SOURCES += liblnk/lnklib.S liblnk/pmon.S liblnk/regs.h

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/interrupts.rel \
    ../../../libcpu/@RTEMS_CPU@/rm52xx/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/rm52xx/vectorisrs.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
