ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/powerpc/ss555/start/bsp_specs

start.$(OBJEXT): ../../../../../../bsps/powerpc/ss555/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# pclock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/ss555/clock/p_clock.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/console/console.c
# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/iss555.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# tm27supp
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/tm27supp.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/btimer/btimer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/exceptions/ppc_exc_print.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/clock/clock.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/dev/console-generic.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/irq_asm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/irq.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/irq_init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/raw_exception.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/vectors_init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/vectors.S

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-generic.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-info.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-lock.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-server.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-shell.c

include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/ss555/headers.am
