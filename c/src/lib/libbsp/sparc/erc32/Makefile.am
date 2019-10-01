ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/sparc/erc32/start/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/sparc/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/sparc/shared/start/linkcmds.base

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/start/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/start/spurious.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/start/erc32mec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/start/boardinit.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/start/bspidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/start/bspdelay.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/start/bsp_fatal_exit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/start/bsp_fatal_halt.c
# gnatsupp
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/gnatsupp/gnatsupp.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/gnatcommon.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/console/erc32_console.c
# debugio
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/console/debugputs.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/sparc/erc32/clock/ckinit.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/btimer/btimer.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/irq/irq-shared.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/irq/bsp_isr_handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c

if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/start/bspsmp.c
endif

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/erc32/net/erc32sonic.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sparc/erc32/headers.am
