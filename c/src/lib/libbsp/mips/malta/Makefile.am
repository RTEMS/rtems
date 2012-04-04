##
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h
include_bsp_HEADERS = ../shared/liblnk/regs.h
#isr
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += include/irq.h
#pci
include_bsp_HEADERS += include/pci.h
#irq
include_bsp_HEADERS += ../shared/irq/i8259.h
nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

nodist_include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S ../shared/liblnk/regs.h
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += startup/simple_access.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bsplibc.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/bsppost.c
libbsp_a_SOURCES += ../../shared/bsppretaskinghook.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += startup/inittlb.c
libbsp_a_SOURCES += ../shared/startup/idttlb.S

# clock
libbsp_a_SOURCES += ../shared/clock/clockdrv.c
libbsp_a_SOURCES += ../shared/clock/mips_timer.S
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h

# console
libbsp_a_SOURCES += console/conscfg.c
libbsp_a_SOURCES += console/printk_support.c
libbsp_a_SOURCES += ../../shared/console.c
libbsp_a_SOURCES += ../../shared/console_select.c
libbsp_a_SOURCES += ../../shared/console_read.c
libbsp_a_SOURCES += ../../shared/console_write.c
libbsp_a_SOURCES += ../../shared/console_control.c
# timer
libbsp_a_SOURCES += ../../shared/timerstub.c

libbsp_a_LIBADD  = ../../../libcpu/@RTEMS_CPU@/shared/cache.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/interrupts.rel

# pci
libbsp_a_SOURCES += pci/pci.c
libbsp_a_SOURCES += pci/pcifinddevice.c
libbsp_a_SOURCES += pci/pcilistdevices.c

#isr
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../shared/irq/vectorexceptions.c
libbsp_a_SOURCES += ../shared/irq/irq.c
libbsp_a_SOURCES += irq/maxvectors.c
libbsp_a_SOURCES += irq/vectorisrs.c
libbsp_a_SOURCES += irq/interruptmask.c
libbsp_a_SOURCES += ../shared/irq/i8259.c

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
