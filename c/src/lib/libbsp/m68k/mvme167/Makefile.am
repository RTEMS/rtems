ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/m68k/mvme167/start/bsp_specs

noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/m68k/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme167/start/bspclean.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme167/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme167/start/page_table.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/m68k/mvme167/clock/ckinit.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme167/console/console.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme167/btimer/btimer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme167/btimer/timerisr.S

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme167/net/network.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/shared/fpsp-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mvme167/headers.am
