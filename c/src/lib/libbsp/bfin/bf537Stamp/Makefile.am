ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/setvec.c
librtemsbsp_a_SOURCES += ../../shared/bootcard.c
librtemsbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c

librtemsbsp_a_SOURCES += console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/bfin/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/bfin/shared/interrupt.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/networkconfig.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/bfin/bf537Stamp/net/ethernet.c
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/bfin/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/bfin/bf537Stamp/headers.am
