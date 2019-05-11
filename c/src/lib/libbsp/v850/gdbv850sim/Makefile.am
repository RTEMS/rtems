##
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/v850/gdbv850sim/start/bsp_specs

noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/v850/gdbv850sim/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

librtemsbsp_a_SOURCES = ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspstart-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/v850/gdbv850sim/start/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/v850/shared/start/crt1.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/v850/gdbv850sim/start/trap.S

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/clock/clock-simidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-polled.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/v850/gdbv850sim/console/console-io.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

project_lib_LIBRARIES = librtemsbsp.a

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/v850/gdbv850sim/headers.am
