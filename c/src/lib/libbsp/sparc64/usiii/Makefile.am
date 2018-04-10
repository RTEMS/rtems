ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

## these are the include files used by the boot process from HelenOS Sparc64
## These include files mimic the HelenOS kernel include layout, which 
## in HelenOS-0.4.2 is more complicated than the boot include dirs.

# assumed by the HelenOS sources to be in the root includedir. We place them
# in a kernel include directory.


noinst_PROGRAMS =



start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += ../shared/startup/linkcmds

project_lib_LIBRARIES = librtemsbsp.a 
librtemsbsp_a_SOURCES =
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/bspstart.c
librtemsbsp_a_SOURCES += ../../shared/bootcard.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
librtemsbsp_a_SOURCES += ../shared/startup/setvec.c
librtemsbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/genarch/balloc.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/genarch/ofw.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/genarch/ofw_tree.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/generic/string.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/sparc64/loader/ofwarch.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/sparc64/loader/main.c
librtemsbsp_a_SOURCES += ../shared/helenos/boot/sparc64/loader/ofwasm.S
librtemsbsp_a_SOURCES += ../shared/asm/asm.S
librtemsbsp_a_SOURCES += start/bspinit.S

librtemsbsp_a_SOURCES += ../shared/helenos/kernel/sparc64/src/cache.S
librtemsbsp_a_SOURCES += ../shared/helenos/kernel/sparc64/src/sun4u/takemmu.S
librtemsbsp_a_SOURCES += ../shared/start/trap_table.S

librtemsbsp_a_SOURCES += ../shared/clock/ckinit.c

librtemsbsp_a_SOURCES += ../shared/console/conscfg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c

librtemsbsp_a_SOURCES += ../../shared/timerstub.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sparc64/usiii/headers.am
