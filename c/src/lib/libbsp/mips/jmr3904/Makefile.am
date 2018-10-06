ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/mips/jmr3904/start/bsp_specs

#isr


start.$(OBJEXT): ../../../../../../bsps/mips/jmr3904/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/jmr3904/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/mips/jmr3904/clock/clockdrv.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-polled.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/jmr3904/console/console-io.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/jmr3904/btimer/btimer.c
#isr
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/vectorexceptions.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/irq.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/jmr3904/irq/vectorisrs.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/interruptmask.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/installisrentries.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/isr_entries.S

include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/mips/jmr3904/headers.am
