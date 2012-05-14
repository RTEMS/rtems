ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

EXTRA_DIST =

include_HEADERS += include/bare.h
include_HEADERS += include/crc.h
include_HEADERS += include/debugport.h
include_HEADERS += include/m68302scc.h
include_HEADERS += include/coverhd.h

# FIXME: Better merge both files into one and use #ifdef GDB_MONITOR_ACTIVE
if ODS68302_DEBUG
EXTRA_DIST += start/debugreset.S
start.$(OBJEXT): start/debugreset.S
	$(CPPASCOMPILE) -o $@ -c $<
else
EXTRA_DIST += start/reset.S
start.$(OBJEXT): start/reset.S
	$(CPPASCOMPILE) -o $@ -c $<
endif

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/rom startup/debugger

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += startup/cpuboot.c startup/crc.c startup/debugport.c \
    startup/gdb-hooks.c ../../shared/bootcard.c \
    startup/m68302scc.c startup/m68k-stub.c \
    ../../shared/bsppretaskinghook.c startup/memcheck.c \
    ../../shared/bspgetworkarea.c startup/memcheck.c \
    startup/trace.c ../../shared/gnatinstallhandler.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../shared/bsppost.c \
    ../../shared/bspstart.c ../../shared/bspclean.c ../../shared/sbrk.c \
    ../../shared/setvec.c
# clock
libbsp_a_SOURCES += clock/ckinit.c
# console
libbsp_a_SOURCES += console/console.c ../../shared/dummy_printk_support.c
# timer
libbsp_a_SOURCES += timer/timer.c timer/timerisr.S

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
