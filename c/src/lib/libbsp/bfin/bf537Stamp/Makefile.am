ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/bfin/bf537Stamp/start/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/bfin/bf537Stamp/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

librtemsbsp_a_SOURCES += ../../../../../../bsps/bfin/bf537Stamp/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/bfin/bf537Stamp/console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/bfin/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/bfin/shared/interrupt.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/bfin/bf537Stamp/net/networkconfig.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/bfin/bf537Stamp/net/ethernet.c
endif

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/bfin/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/bfin/bf537Stamp/headers.am
