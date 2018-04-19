ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../shared/startup/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += startup/setvec.c
librtemsbsp_a_SOURCES += startup/spurious.c
librtemsbsp_a_SOURCES += startup/erc32mec.c
librtemsbsp_a_SOURCES += startup/boardinit.S
librtemsbsp_a_SOURCES += startup/bspidle.c
librtemsbsp_a_SOURCES += startup/bspdelay.c
librtemsbsp_a_SOURCES += ../shared/startup/bsp_fatal_exit.c
librtemsbsp_a_SOURCES += ../shared/startup/bsp_fatal_halt.c
# ISR Handler
librtemsbsp_a_SOURCES += ../shared/cpu.c
librtemsbsp_a_SOURCES += ../shared/irq_asm.S
# gnatsupp
librtemsbsp_a_SOURCES += gnatsupp/gnatsupp.c
librtemsbsp_a_SOURCES += ../shared/gnatcommon.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c
librtemsbsp_a_SOURCES += console/erc32_console.c
# debugio
librtemsbsp_a_SOURCES += console/debugputs.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/sparc/erc32/clock/ckinit.c
# timer
librtemsbsp_a_SOURCES += timer/timer.c

# IRQ
librtemsbsp_a_SOURCES += ../shared/irq/irq-shared.c
librtemsbsp_a_SOURCES += ../shared/irq/bsp_isr_handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c

if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspsmp-dummy.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/getcurrentprocessor-zero.c
endif

if HAS_NETWORKING
librtemsbsp_a_SOURCES += erc32sonic/erc32sonic.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sparc/erc32/headers.am
