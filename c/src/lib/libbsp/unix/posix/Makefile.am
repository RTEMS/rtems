##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

EXEEXT =

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

nodist_include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../shared/bsppretaskinghook.c \
    startup/bspstart.c startup/bsppost.c startup/bspgetworkarea.c \
    startup/setvec.c ../../shared/bootcard.c
libbsp_a_SOURCES += startup/main.c

# clock
libbsp_a_SOURCES += clock/clock.c
# console
libbsp_a_SOURCES += console/console.c console/console-io.c
# timer
libbsp_a_SOURCES += timer/timer.c

if HAS_MP
# shmsupp
libbsp_a_SOURCES += shmsupp/addrconv.c shmsupp/getcfg.c \
    shmsupp/cause_intr.c shmsupp/lock.c shmsupp/mpisr.c
endif

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
