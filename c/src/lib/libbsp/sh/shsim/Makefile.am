ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/sh/shsim/start/bsp_specs



start.$(OBJEXT): ../../../../../../bsps/sh/shsim/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shared/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shared/start/bsphwinit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shsim/start/sysexit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shsim/start/cpu_asm.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shsim/start/ispshgdb.c

# clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/clock/clock-simidle.c

# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-polled.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shsim/console/console-io.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shsim/console/console-support.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shsim/console/console-debugio.c

# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sh/shsim/headers.am
