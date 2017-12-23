##
## $Id: Makefile.am,v 1.36 2008/10/02 21:39:40 joel Exp $
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h


EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/bspgetworkarea.c \
    ../../shared/bspstart.c ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/bspreset.c
# clock
libbsp_a_SOURCES += ../../shared/clock_driver_simidle.c
# console
libbsp_a_SOURCES += ../../shared/console-polled.c console/console-io.c \
   console/syscalls.S
# timer
libbsp_a_SOURCES += ../../shared/timerstub.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../shared/include/cache_.h
libbsp_a_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/../../shared/include

EXTRA_DIST += times

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/moxie/moxiesim/headers.am
