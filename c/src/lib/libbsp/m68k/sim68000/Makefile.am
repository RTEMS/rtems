ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

nodist_include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bspstart.c ../../shared/bootcard.c \
    startup/bsppredriverhook.c ../../shared/bsppretaskinghook.c \
    ../../shared/bspgetworkarea.c ../../shared/sbrk.c \
    ../../shared/setvec.c ../../shared/gnatinstallhandler.c \
    ../../m68k/shared/bspspuriousinit.c ../../m68k/shared/m68000spurious.c \
    startup/spurious_assistant.c
# clock
libbsp_a_SOURCES += clock/clockdrv.c ../../../shared/clockdrv_shell.h
# console
libbsp_a_SOURCES += console/conscfg.c ../../shared/console.c \
    ../../shared/console_select.c ../../shared/console_control.c \
    ../../shared/console_read.c ../../shared/console_write.c
# debugio
libbsp_a_SOURCES += console/debugio.c
# timer
libbsp_a_SOURCES += ../../shared/timerstub.c

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
