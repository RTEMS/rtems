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
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/mips/csb350/clock/clockdrv.c
# console
librtemsbsp_a_SOURCES += console/console-io.c
librtemsbsp_a_SOURCES += ../../shared/console-polled.c
# timer
librtemsbsp_a_SOURCES += timer/timer.c
#isr
librtemsbsp_a_SOURCES += ../shared/irq/vectorexceptions.c
librtemsbsp_a_SOURCES += ../shared/irq/irq.c
librtemsbsp_a_SOURCES += irq/vectorisrs.c
librtemsbsp_a_SOURCES += ../shared/irq/interruptmask.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/network.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/installisrentries.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/isr_entries.S

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/mips/csb350/headers.am
