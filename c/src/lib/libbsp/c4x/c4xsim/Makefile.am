##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += include/simio.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/bsppost.c startup/bspstart.c \
    ../../shared/bootcard.c ../../shared/sbrk.c ../../shared/setvec.c \
    ../../c4x/shared/c3xspurious.c ../../c4x/shared/c4xspurious.c \
    ../../c4x/shared/bspspuriousinit.c startup/spurious.c \
    ../../shared/gnatinstallhandler.c
clock_SOURCES = clock/clock.c
console_SOURCES = console/consolereserveresources.c console/debugio.c \
    console/simio.c ../../shared/console.c
timer_SOURCES = timer/timer.c

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(timer_SOURCES)

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
