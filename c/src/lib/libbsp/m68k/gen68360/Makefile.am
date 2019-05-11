ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/m68k/gen68360/start/bsp_specs

noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/m68k/gen68360/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/m68k/gen68360/start/linkcmds.bootp
dist_project_lib_DATA += ../../../../../../bsps/m68k/gen68360/start/linkcmds.prom

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/gen68360/start/alloc360.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspstart-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/gen68360/start/init68360.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/m68k/gen68360/clock/clock.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/gen68360/console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/printk-dummy.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/gen68360/btimer/btimer.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/gen68360/net/network.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c

if HAS_FPSP
include $(srcdir)/../../../../../../bsps/m68k/shared/fpsp-sources.am
endif

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/gen68360/headers.am
