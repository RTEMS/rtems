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
noinst_PROGRAMS =

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

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c startup/bspstart.c \
    startup/setvec.c ../../shared/bootcard.c
## for now always using main.c style startup
##if !HAS_CXX
startup_rel_SOURCES += startup/main.c
##endif

startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/clock.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c console/console-io.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_MP
noinst_PROGRAMS += shmsupp.rel
shmsupp_rel_SOURCES = shmsupp/addrconv.c shmsupp/getcfg.c \
    shmsupp/cause_intr.c shmsupp/lock.c shmsupp/mpisr.c
shmsupp_rel_CPPFLAGS = $(AM_CPPFLAGS)
shmsupp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =
## if HAS_CXX
## libbsp_a_SOURCES += startup/rtems-ctor.cc
## endif
libbsp_a_LIBADD = startup.rel clock.rel console.rel timer.rel
if HAS_MP
libbsp_a_LIBADD += shmsupp.rel
endif

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
