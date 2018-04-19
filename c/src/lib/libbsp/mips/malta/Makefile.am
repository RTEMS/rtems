##
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

#isr
#pci
#irq


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += startup/simple_access.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += startup/inittlb.c
librtemsbsp_a_SOURCES += ../shared/startup/idttlb.S

# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/mips/shared/clock/clockdrv.c
librtemsbsp_a_SOURCES +=../../../../../../bsps/mips/shared/clock/mips_timer.S

# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c
librtemsbsp_a_SOURCES += console/conscfg.c
librtemsbsp_a_SOURCES += console/printk_support.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/installisrentries.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/isr_entries.S

# pci
librtemsbsp_a_SOURCES += pci/pci.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/pci/pci_find_device.c

#isr
librtemsbsp_a_SOURCES += ../shared/irq/vectorexceptions.c
librtemsbsp_a_SOURCES += ../shared/irq/irq.c
librtemsbsp_a_SOURCES += irq/vectorisrs.c
librtemsbsp_a_SOURCES += irq/interruptmask.c
librtemsbsp_a_SOURCES += ../shared/irq/i8259.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/mips/malta/headers.am
