
ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

# include

# start
rtems_crti.$(OBJEXT): ../shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a

# startup
librtemsbsp_a_SOURCES = startup/bspclean.c
librtemsbsp_a_SOURCES += ../shared/startup/zerobss.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../shared/src/memcpy.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c

# start
librtemsbsp_a_SOURCES += start/start.S

# console
librtemsbsp_a_SOURCES += startup/dummy_console.c
librtemsbsp_a_SOURCES += ../../shared/dummy_printk_support.c

# irq
librtemsbsp_a_SOURCES += irq/irq_init.c

# mmu
librtemsbsp_a_SOURCES += mmu/mmu.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/clock-ppc403.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/virtex4/headers.am
