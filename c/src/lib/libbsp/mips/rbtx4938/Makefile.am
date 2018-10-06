ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/mips/rbtx4938/start/bsp_specs

#isr

noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/mips/rbtx4938/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/rbtx4938/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/rbtx4938/start/inittlb.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/start/idttlb.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/start/idtmem.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/exception.S
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/mips/rbtx4938/clock/clockdrv.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/rbtx4938/console/console-io.c
# liblnk
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/liblnk/lnklib.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/liblnk/pmon.S
#isr
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/vectorexceptions.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/irq.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/rbtx4938/irq/vectorisrs.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/interruptmask_TX49.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/installisrentries.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/isr_entries.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/timer/timer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/timer/gettime.S

include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/mips/rbtx4938/headers.am
