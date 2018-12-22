ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/sparc/leon2/start/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/sparc/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/sparc/shared/start/linkcmds.base
dist_project_lib_DATA += ../../../../../../bsps/sparc/leon2/start/linkcmds.at697f
dist_project_lib_DATA += ../../../../../../bsps/sparc/leon2/start/linkcmds.leon2

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/start/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/start/spurious.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/start/bspidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/start/bspdelay.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/start/bsp_fatal_exit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/start/bsp_fatal_halt.c

# gnatsupp
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/gnatsupp/gnatsupp.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/gnatcommon.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/console/debugputs.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/sparc/leon2/clock/ckinit.c
# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/irq/irq-shared.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/irq/bsp_isr_handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c

# PCI
librtemsbsp_a_SOURCES +=  ../../../../../../bsps/sparc/leon2/pci/at697_pci.c
# l2cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/start/cache.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/btimer/btimer.c

# Driver Manager
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/drvmgr/ambapp_bus_leon2.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/drvmgr/leon2_amba_bus.c

if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspsmp-dummy.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/getcurrentprocessor-zero.c
endif

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/net/leon_smc91111.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/net/leon_open_eth.c
endif


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/grlib-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sparc/leon2/headers.am
