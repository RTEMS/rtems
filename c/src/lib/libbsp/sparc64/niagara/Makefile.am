ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

noinst_PROGRAMS =



start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += ../shared/startup/linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =
librtemsbsp_a_SOURCES += startup/bspclean.c
librtemsbsp_a_SOURCES += startup/m5op_sparc.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspstart-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../shared/startup/setvec.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/genarch/balloc.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/genarch/ofw.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/genarch/ofw_tree.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/generic/string.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/sparc64/loader/ofwarch.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/sparc64/loader/main.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/sparc64/loader/ofwasm.S
librtemsbsp_a_SOURCES += start/bspinit.S
librtemsbsp_a_SOURCES += ../shared/asm/asm.S

librtemsbsp_a_SOURCES +=../../../../../../bsps/sparc64/shared/clock/ckinit.c

librtemsbsp_a_SOURCES += ../shared/console/conscfg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sparc64/niagara/headers.am
