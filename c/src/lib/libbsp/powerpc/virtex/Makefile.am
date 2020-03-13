ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/powerpc/virtex/start/bsp_specs

if HAS_NETWORKING
endif


noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/powerpc/virtex/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

# Link commands
project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/powerpc/shared/start/linkcmds.base

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =
librtemsbsp_a_LIBADD =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-loop.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/virtex/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/bsp-start-zero.S

# bspconsole
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/virtex/console/consolelite.c

# irq
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/virtex/irq/irq_init.c

# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/shared/clock/clock.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += =
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/virtex/net/xiltemac.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/btimer/btimer-ppc-dec.c

include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/virtex/headers.am
