ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

#isr
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =


EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bspreset.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += startup/inittlb.c
libbsp_a_SOURCES += ../shared/startup/idttlb.S
libbsp_a_SOURCES += ../shared/startup/idtmem.S
libbsp_a_SOURCES += ../shared/irq/exception.S
# clock
libbsp_a_SOURCES += clock/clockdrv.c
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
# console
libbsp_a_SOURCES += console/console-io.c
# liblnk
libbsp_a_SOURCES += ../shared/liblnk/lnklib.S
libbsp_a_SOURCES += ../shared/liblnk/pmon.S
libbsp_a_SOURCES += ../shared/liblnk/regs.h
#isr
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../shared/irq/vectorexceptions.c
libbsp_a_SOURCES += ../shared/irq/irq.c
libbsp_a_SOURCES += irq/vectorisrs.c
libbsp_a_SOURCES += ../shared/irq/interruptmask_TX49.c

libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c
libbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/installisrentries.c
libbsp_a_SOURCES += ../../../../../../bsps/mips/shared/irq/isr_entries.S
libbsp_a_SOURCES += ../../../../../../bsps/mips/shared/timer/timer.c
libbsp_a_SOURCES += ../../../../../../bsps/mips/shared/timer/gettime.S

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/mips/rbtx4925/headers.am
