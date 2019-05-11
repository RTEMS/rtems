ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/nios2/nios2_iss/start/bsp_specs



start.$(OBJEXT): ../../../../../../bsps/nios2/nios2_iss/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
crtnn.$(OBJEXT): ../../../../../../bsps/nios2/nios2_iss/start/crtnn.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT) crtnn.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-loop.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/nios2/nios2_iss/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/nios2/nios2_iss/start/setvec.c

# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/nios2/nios2_iss/clock/clock.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/nios2/nios2_iss/console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-polled.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/nios2/nios2_iss/btimer/btimer.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/nios2/nios2_iss/headers.am
