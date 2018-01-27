ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =


EXTRA_DIST += ../../sparc/shared/start/start.S
start.$(OBJEXT): ../../sparc/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += startup/bsppredriver.c
libbsp_a_SOURCES += ../../sparc/shared/startup/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bspreset.c
libbsp_a_SOURCES += ../../shared/bspstart.c
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += startup/setvec.c
libbsp_a_SOURCES += startup/spurious.c
libbsp_a_SOURCES += startup/erc32mec.c
libbsp_a_SOURCES += startup/boardinit.S
libbsp_a_SOURCES += startup/bspidle.c
libbsp_a_SOURCES += startup/bspdelay.c
libbsp_a_SOURCES += ../../sparc/shared/startup/bsp_fatal_exit.c
libbsp_a_SOURCES += ../../sparc/shared/startup/bsp_fatal_halt.c
# ISR Handler
libbsp_a_SOURCES += ../../sparc/shared/cpu.c
libbsp_a_SOURCES += ../../sparc/shared/irq_asm.S
# gnatsupp
libbsp_a_SOURCES += gnatsupp/gnatsupp.c ../../sparc/shared/gnatcommon.c
# console
libbsp_a_SOURCES += console/erc32_console.c ../../shared/console.c \
    ../../shared/console_select.c  ../../shared/console_control.c \
    ../../shared/console_read.c ../../shared/console_write.c
# debugio
libbsp_a_SOURCES += console/debugputs.c
# clock
libbsp_a_SOURCES += clock/ckinit.c
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
# timer
libbsp_a_SOURCES += timer/timer.c

# IRQ
libbsp_a_SOURCES += \
    ../../sparc/shared/irq/irq-shared.c \
    ../../sparc/shared/irq/bsp_isr_handler.c \
    ../../shared/src/irq-default-handler.c \
    ../../shared/src/irq-generic.c \
    ../../shared/src/irq-info.c \
    ../../shared/src/irq-legacy.c \
    ../../shared/src/irq-server.c \
    ../../shared/src/irq-shell.c

if HAS_SMP
libbsp_a_SOURCES += ../../shared/bspsmp.c
libbsp_a_SOURCES += ../../shared/bspsmpgetcurrentprocessor.c
endif

if HAS_NETWORKING
libbsp_a_SOURCES += erc32sonic/erc32sonic.c
endif

libbsp_a_LIBADD  = \
    ../../../libcpu/@RTEMS_CPU@/access.rel \
    ../../../libcpu/@RTEMS_CPU@/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/reg_win.rel \
    ../../../libcpu/@RTEMS_CPU@/syscall.rel

EXTRA_DIST += times

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/sparc/erc32/headers.am
