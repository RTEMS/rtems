##
## Makefile.am,v 1.8.4.1 2003/02/20 21:55:34 joel Exp
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

include_bspdir = $(includedir)/bsp

###
dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c \
    ../../powerpc/shared/showbats.c  \
    ../../shared/bsppost.c ../../shared/bsppredriverhook.c 


pclock_SOURCES = ../../powerpc/shared/clock/p_clock.c
console_SOURCES = console/console.c console/ns16550cfg.c \
    console/mc68360_scc.c console/rsPMCQ1.c console/alloc360.c \
    console/init68360.c

include_bsp_HEADERS = ../../powerpc/shared/pci/pci.h \
    ../../powerpc/shared/motorola/motorola.h \
     ../../powerpc/shared/residual/residual.h \
    ../../powerpc/shared/residual/pnp.h \
    ../../powerpc/shared/console/consoleIo.h console/rsPMCQ1.h

pci_SOURCES = pci/no_host_bridge.c ../../powerpc/shared/pci/pci.c \
        ../../powerpc/shared/pci/pcifinddevice.c

include_bsp_HEADERS += irq/irq.h \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/ppc_exc_bspsupp.h \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/vectors.h \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h

irq_SOURCES =  irq/irq.h  irq/irq_init.c irq/openpic_xxx_irq.c \
    ../../powerpc/shared/irq/i8259.c 
    
include_bsp_HEADERS += ../../powerpc/shared/openpic/openpic.h

openpic_SOURCES = ../../powerpc/shared/openpic/openpic.c

include_bsp_HEADERS += ../../shared/vmeUniverse/vmeUniverse.h \
    vme/VMEConfig.h \
    ../../shared/vmeUniverse/vme_am_defs.h \
    ../../shared/vmeUniverse/VME.h \
    ../../shared/vmeUniverse/vmeUniverseDMA.h\
    ../../shared/vmeUniverse/bspVmeDmaList.h\
    ../../shared/vmeUniverse/VMEDMA.h

vme_SOURCES = ../../shared/vmeUniverse/vmeUniverse.c \
    ../../shared/vmeUniverse/bspVmeDmaList.c \
    vme/vmeconfig.c \
    ../shared/vme/vme_universe.c \
    ../../shared/vmeUniverse/vme_am_defs.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

EXTRA_DIST += README

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =  $(pclock_SOURCES) $(console_SOURCES) $(irq_SOURCES) \
    $(pci_SOURCES) $(vectors_SOURCES) $(startup_SOURCES) \
    $(openpic_SOURCES) $(vme_SOURCES) 

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
