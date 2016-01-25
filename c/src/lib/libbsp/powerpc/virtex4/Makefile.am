
ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

# include
include_HEADERS     = include/bsp.h
include_HEADERS    += ../../shared/include/tm27.h

include_bsp_HEADERS =

nodist_include_HEADERS     = include/bspopts.h
nodist_include_HEADERS    += include/coverhd.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES             = include/bspopts.h

# start
noinst_LIBRARIES      = libbspstart.a
libbspstart_a_SOURCES = ../../powerpc/shared/start/rtems_crti.S
project_lib_DATA      = rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a

# startup
libbsp_a_SOURCES = startup/bspclean.c \
                   ../shared/startup/zerobss.c \
                   ../../shared/bspgetworkarea.c \
                   startup/bspstart.c \
                   ../../shared/bootcard.c \
                   ../../shared/sbrk.c \
                   ../../shared/gnatinstallhandler.c \
                   ../shared/src/memcpy.c

# start
libbsp_a_SOURCES += start/start.S

# console
libbsp_a_SOURCES += startup/dummy_console.c \
                    ../../shared/dummy_printk_support.c

# irq
include_bsp_HEADERS += include/irq.h
libbsp_a_SOURCES += irq/irq_init.c

# mmu
include_bsp_HEADERS += include/mmu.h
libbsp_a_SOURCES += mmu/mmu.c

#vectors
include_bsp_HEADERS += ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/vectors.h
include_bsp_HEADERS += ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
                  ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
                  ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
                  ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
                  ../../../libcpu/@RTEMS_CPU@/ppc403/clock.rel \
                  ../../../libcpu/@RTEMS_CPU@/ppc403/timer.rel

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
