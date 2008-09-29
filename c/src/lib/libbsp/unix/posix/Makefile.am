##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

EXEEXT =

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

nodist_include_HEADERS += ../../shared/include/coverhd.h

project_lib_DATA =
noinst_LIBRARIES =
## if HAS_CXX
## noinst_LIBRARIES += librtemscxx.a
## 
## librtemscxx_a_SOURCES = startup/no-ctor.c
## 
## # Install as a separate .$(OBJEXT)
## project_lib_DATA += no-ctor.$(OBJEXT)
## 
## # We install the RTEMS constructor as a separate .$(OBJEXT)
## # so it can be easily place correctly by the compiler config file.
## project_lib_DATA += rtems-ctor.$(OBJEXT)
## endif

startup_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c startup/bspstart.c \
    startup/setvec.c ../../shared/bootcard.c
## for now always using main.c style startup
##if !HAS_CXX
startup_SOURCES += startup/main.c
##endif

clock_SOURCES = clock/clock.c
console_SOURCES = console/console.c console/console-io.c
timer_SOURCES = timer/timer.c

if HAS_MP
shmsupp_SOURCES = shmsupp/addrconv.c shmsupp/getcfg.c \
    shmsupp/cause_intr.c shmsupp/lock.c shmsupp/mpisr.c
endif

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) \
    $(console_SOURCES) $(timer_SOURCES) $(shmsupp_SOURCES)
## if HAS_CXX
## libbsp_a_SOURCES += startup/rtems-ctor.cc
## endif

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
