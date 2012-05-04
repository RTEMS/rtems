ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

include_bsp_HEADERS =

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
libbsp_a_SOURCES += ../../shared/bsplibc.c ../../shared/bsppost.c \
  ../../shared/bsppredriverhook.c ../../shared/bspgetworkarea.c \
  ../../shared/bootcard.c ../../shared/sbrk.c \
  ../../shared/gnatinstallhandler.c ../../shared/bsppretaskinghook.c \
  ../../shared/bspclean.c startup/bspstart.c startup/memmap.c

#console
libbsp_a_SOURCES += console/uarts.c ../../shared/console.c \
  ../../shared/console_select.c  ../../shared/console_control.c \
  ../../shared/console_read.c ../../shared/console_write.c 
# IRQ
include_bsp_HEADERS += ../../shared/include/irq-generic.h \
	../../shared/include/irq-info.h
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
#abort
libbsp_a_SOURCES += ../shared/abort/abort.c
#framebuffer
if ON_SKYEYE
libbsp_a_SOURCES += fb/fb.c
if HAS_NETWORKING
rtl8019_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += rtl8019.rel
rtl8019_rel_SOURCES = rtl8019/rtl8019.c rtl8019/wd80x3.h
rtl8019_rel_CPPFLAGS = $(AM_CPPFLAGS) $(rtl8019_CPPFLAGS)
rtl8019_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/arm920.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/ffuart.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/irq.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/pmc.rel

if ON_SKYEYE
if HAS_NETWORKING
libbsp_a_LIBADD += rtl8019.rel
endif
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
