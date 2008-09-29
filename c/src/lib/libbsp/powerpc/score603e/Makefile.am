##
## $Id$
##

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
include_HEADERS += include/coverhd.h include/gen2.h

###
dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES = libbsp.a

startup_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    startup/bspstart.c ../../powerpc/shared/startup/bspgetworkarea.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bootcard.c \
    startup/Hwr_init.c startup/genpvec.c ../../shared/gnatinstallhandler.c \
    ../../powerpc/shared/showbats.c \
    ../../shared/bsppretaskinghook.c

pclock_SOURCES = ../../powerpc/shared/clock/p_clock.c
console_SOURCES = console/85c30.c console/85c30.h console/console.c \
    console/tbl85c30.c console/consolebsp.h

include_bsp_HEADERS = ../../powerpc/shared/pci/pci.h \
    PCI_bus/PCI.h \
    ../../powerpc/shared/residual/residual.h \
    ../../powerpc/shared/residual/pnp.h \
    ../../powerpc/shared/openpic/openpic.h \
    ../../powerpc/shared/console/consoleIo.h
pci_SOURCES = pci/no_host_bridge.c ../../powerpc/shared/pci/pci.c \
    ../../powerpc/shared/pci/pcifinddevice.c  PCI_bus/PCI.c  PCI_bus/universe.c

include_bsp_HEADERS += irq/irq.h \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/ppc_exc_bspsupp.h \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/vectors.h \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h
irq_SOURCES = irq/FPGA.c irq/irq.c ../../powerpc/shared/irq/irq_asm.S

include_bsp_HEADERS += ../../powerpc/shared/vectors/vectors.h
vectors_SOURCES = ../../powerpc/shared/vectors/vectors_init.c \
    ../../powerpc/shared/vectors/vectors.S

include_bsp_HEADERS += ../../shared/vmeUniverse/vmeUniverse.h \
    ../../shared/vmeUniverse/vme_am_defs.h \
    ../../shared/vmeUniverse/VME.h \
    vme/VMEConfig.h \
    ../../powerpc/shared/motorola/motorola.h \
    ../../shared/vmeUniverse/vmeUniverseDMA.h\
    ../../shared/vmeUniverse/bspVmeDmaList.h\
    ../../shared/vmeUniverse/VMEDMA.h
vme_SOURCES = \
    ../../shared/vmeUniverse/bspVmeDmaList.c \
    ../../shared/vmeUniverse/vme_am_defs.h \
	../shared/vme/vmeconfig.c 

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

libbsp_a_SOURCES = $(pclock_SOURCES) $(console_SOURCES) $(irq_SOURCES) \
    $(pci_SOURCES) $(vectors_SOURCES) $(startup_SOURCES) $(vme_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
     ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
