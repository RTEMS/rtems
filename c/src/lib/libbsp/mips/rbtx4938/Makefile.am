ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

#isr

noinst_PROGRAMS =


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_CPPFLAGS = $(AM_CPPFLAGS) -DTX49
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += ../../shared/bootcard.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += startup/inittlb.c
librtemsbsp_a_SOURCES += ../shared/startup/idttlb.S
librtemsbsp_a_SOURCES += ../shared/startup/idtmem.S
librtemsbsp_a_SOURCES += ../shared/irq/exception.S
# clock
librtemsbsp_a_SOURCES += clock/clockdrv.c
# console
librtemsbsp_a_SOURCES += console/console-io.c
# liblnk
librtemsbsp_a_SOURCES += ../shared/liblnk/lnklib.S
librtemsbsp_a_SOURCES += ../shared/liblnk/pmon.S
#isr
librtemsbsp_a_SOURCES += ../shared/irq/vectorexceptions.c
librtemsbsp_a_SOURCES += ../shared/irq/irq.c
librtemsbsp_a_SOURCES += irq/vectorisrs.c
librtemsbsp_a_SOURCES += ../shared/irq/interruptmask_TX49.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/installisrentries.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/isr_entries.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/timer/timer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/timer/gettime.S

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/mips/rbtx4938/headers.am
