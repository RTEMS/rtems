ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

#isr


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_CPPFLAGS = $(AM_CPPFLAGS) -DRM52XX
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += startup/inittlb.c
librtemsbsp_a_SOURCES += ../shared/startup/idtmem.S
librtemsbsp_a_SOURCES += ../shared/startup/idttlb.S
librtemsbsp_a_SOURCES += ../shared/irq/exception.S
librtemsbsp_a_SOURCES += startup/usc.S
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/mips/hurricane/clock/ckinit.c

# console
librtemsbsp_a_SOURCES += console/console.c
# liblnk
librtemsbsp_a_SOURCES += ../shared/liblnk/lnklib.S
librtemsbsp_a_SOURCES += ../shared/liblnk/pmon.S
#isr
librtemsbsp_a_SOURCES += ../shared/irq/vectorexceptions.c
librtemsbsp_a_SOURCES += ../shared/irq/irq.c
librtemsbsp_a_SOURCES += irq/vectorisrs.c
librtemsbsp_a_SOURCES += ../shared/irq/interruptmask.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/installisrentries.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/isr_entries.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/timer/timer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/timer/gettime.S

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/mips/hurricane/headers.am
