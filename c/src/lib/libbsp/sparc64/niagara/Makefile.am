ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/sparc64/niagara/start/bsp_specs

noinst_PROGRAMS =



start.$(OBJEXT): ../../../../../../bsps/sparc64/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += ../../../../../../bsps/sparc64/shared/start/linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/niagara/start/bspclean.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/niagara/start/m5op_sparc.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspstart-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/boot/genarch/balloc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/boot/genarch/ofw.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/boot/genarch/ofw_tree.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/boot/sparc64/loader/ofwarch.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/boot/sparc64/loader/main.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/boot/sparc64/loader/ofwasm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/niagara/start/bspinit.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/start/halt.S

librtemsbsp_a_SOURCES +=../../../../../../bsps/sparc64/shared/clock/ckinit.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/console/conscfg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sparc64/niagara/headers.am
