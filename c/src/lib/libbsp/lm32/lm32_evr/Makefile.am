ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += include/coverhd.h
include_HEADERS += include/system_conf.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = ../../lm32/shared/start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../shared/startup/bspstart.c \
    ../../shared/bspreset.c ../../shared/bsppretaskinghook.c \
    ../../shared/bspgetworkarea.c ../../shared/bootcard.c \
    ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
# clock
libbsp_a_SOURCES += ../../lm32/shared/clock/ckinit.c
# console
libbsp_a_SOURCES += ../../lm32/shared/console/console.c ../../lm32/shared/console/uart.c
# timer
libbsp_a_SOURCES += ../../lm32/shared/timer/timer.c

if HAS_NETWORKING
noinst_PROGRAMS += network.rel
network_rel_SOURCES = ../../lm32/shared/tsmac/tsmac.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
libbsp_a_LIBADD += network.rel
endif


include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
