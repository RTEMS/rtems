ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/sh/gensh2/start/bsp_specs



start.$(OBJEXT): ../../../../../../bsps/sh/gensh2/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/sh/gensh2/start/linkcmds.ram
dist_project_lib_DATA += ../../../../../../bsps/sh/gensh2/start/linkcmds.rom

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh2/start/hw_init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shared/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shared/start/bsphwinit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES +=../../../../../../bsps/sh/gensh2/clock/ckinit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh2/start/cpu_asm.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh2/start/ispsh7045.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh2/btimer/btimer.c
# scitab
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh2/console/scitab.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shared/console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh2/console/config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh2/console/sci.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh2/console/sci_termios.c
# debugio
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/printk-dummy.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sh/gensh2/headers.am
