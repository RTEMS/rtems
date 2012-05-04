ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

include_HEADERS += ../../shared/tod.h
include_HEADERS += ../../shared/include/coverhd.h include/gen2.h

###
dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    startup/bspstart.c ../../powerpc/shared/startup/bspgetworkarea.c \
    ../../shared/bsppost.c  \
    ../../shared/bootcard.c ../../shared/bsppretaskinghook.c \
    startup/Hwr_init.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/gnatinstallhandler.c \
    ../../powerpc/shared/showbats.c

pclock_SOURCES = ../../powerpc/shared/clock/p_clock.c
console_SOURCES = console/85c30.c console/85c30.h console/console.c \
    console/tbl85c30.c console/consolebsp.h

include_bsp_HEADERS = ../../powerpc/shared/pci/pci.h \
	PCI_bus/PCI.h \
	../../powerpc/shared/residual/residual.h \
	../../powerpc/shared/residual/pnp.h \
	../../powerpc/shared/console/consoleIo.h

pci_SOURCES = PCI_bus/flash.c ../../powerpc/shared/pci/pci.c \
    ../../powerpc/shared/pci/pcifinddevice.c  PCI_bus/PCI.c  PCI_bus/universe.c

include_bsp_HEADERS += irq/irq.h \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h

irq_SOURCES =  irq/no_pic.c irq/irq_init.c \
    irq/FPGA.c

vectors_SOURCES =

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(pclock_SOURCES) $(console_SOURCES) $(irq_SOURCES) \
    $(pci_SOURCES) $(vectors_SOURCES) $(startup_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel \
     ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
