ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/sparc64/usiii/start/bsp_specs

## these are the include files used by the boot process from HelenOS Sparc64
## These include files mimic the HelenOS kernel include layout, which 
## in HelenOS-0.4.2 is more complicated than the boot include dirs.

# assumed by the HelenOS sources to be in the root includedir. We place them
# in a kernel include directory.


noinst_PROGRAMS =



start.$(OBJEXT): ../../../../../../bsps/sparc64/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += ../../../../../../bsps/sparc64/shared/start/linkcmds

project_lib_LIBRARIES = librtemsbsp.a 
librtemsbsp_a_SOURCES =
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspstart-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/boot/genarch/balloc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/boot/genarch/ofw.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/boot/genarch/ofw_tree.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/boot/sparc64/loader/ofwarch.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/boot/sparc64/loader/main.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/boot/sparc64/loader/ofwasm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/start/halt.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/usiii/start/bspinit.S

librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/kernel/sparc64/src/cache.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/helenos/kernel/sparc64/src/sun4u/takemmu.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/start/trap_table.S

librtemsbsp_a_SOURCES +=../../../../../../bsps/sparc64/shared/clock/ckinit.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc64/shared/console/conscfg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sparc64/usiii/headers.am
