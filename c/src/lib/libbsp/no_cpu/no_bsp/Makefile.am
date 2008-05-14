##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

include_HEADERS += include/coverhd.h

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c startup/main.c \
    ../../shared/bootcard.c ../../shared/sbrk.c startup/setvec.c \
    ../../shared/gnatinstallhandler.c
clock_SOURCES = clock/ckinit.c
console_SOURCES = console/console.c
timer_SOURCES = timer/timer.c timer/timerisr.c
shmsupp_SOURCES = shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c \
    shmsupp/mpisr.c

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(timer_SOURCES) $(shmsupp_SOURCES)

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
