ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/powerpc/virtex5/start/bsp_specs

# include

# start

start.$(OBJEXT): ../../../../../../bsps/powerpc/virtex5/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a

# startup
librtemsbsp_a_SOURCES = ../../../../../../bsps/powerpc/virtex5/start/bspclean.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/zerobss.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/virtex5/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/memcpy.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c

# start
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/virtex5/start/start.S

# clock & timer
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/shared/clock/p_clock.c

# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/virtex5/start/dummy_console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/printk-dummy.c

# irq
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/virtex5/irq/irq_init.c

# mmu
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/virtex5/start/mmu.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/clock/clock-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/btimer/btimer-ppc-dec.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/virtex5/headers.am
