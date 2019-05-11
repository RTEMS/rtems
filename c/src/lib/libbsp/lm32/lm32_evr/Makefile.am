ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/lm32/lm32_evr/start/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/lm32/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =
librtemsbsp_a_LIBADD =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/lm32/shared/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/lm32/shared/start/bspreset.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/lm32/shared/clock/ckinit.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/lm32/shared/console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/lm32/shared/console/uart.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/lm32/shared/btimer/btimer.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/lm32/shared/net/tsmac.c
endif

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/lm32/lm32_evr/headers.am
