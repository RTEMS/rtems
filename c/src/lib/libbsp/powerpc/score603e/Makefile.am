##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

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

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

libbspstart_a_SOURCES += ../../powerpc/shared/start/rtems_crti.S
project_lib_DATA += rtems_crti.$(OBJEXT)

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += startup/bspclean.c ../../shared/bsplibc.c \
    startup/bspstart.c ../../powerpc/shared/startup/bspgetworkarea.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bootcard.c \
    startup/Hwr_init.c startup/genpvec.c ../../shared/gnatinstallhandler.c \
    ../../powerpc/shared/showbats.c \
    ../../shared/bsppretaskinghook.c

# pclock
libbsp_a_SOURCES += ../../powerpc/shared/clock/p_clock.c
# console
libbsp_a_SOURCES += console/85c30.c console/85c30.h console/console.c \
    console/tbl85c30.c console/consolebsp.h

include_bsp_HEADERS = ../../powerpc/shared/pci/pci.h \
    PCI_bus/PCI.h ../../powerpc/shared/residual/residual.h \
    ../../powerpc/shared/residual/pnp.h \
    ../../powerpc/shared/openpic/openpic.h \
    ../../powerpc/shared/console/consoleIo.h \
    ../../shared/include/irq-generic.h include/irq-config.h
# pci
libbsp_a_SOURCES += pci/no_host_bridge.c ../../powerpc/shared/pci/pci.c \
    ../../powerpc/shared/pci/pcifinddevice.c  PCI_bus/PCI.c  PCI_bus/universe.c

include_bsp_HEADERS += irq/irq.h \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/ppc_exc_bspsupp.h \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h
# irq
libbsp_a_SOURCES += include/irq-config.h irq/FPGA.c irq/irq.c  \
    ../../shared/src/irq-generic.c ../../powerpc/shared/irq/irq_asm.S

include_bsp_HEADERS += ../../powerpc/shared/vectors/vectors.h
# vectors
libbsp_a_SOURCES += ../../powerpc/shared/vectors/vectors_init.c \
    ../../powerpc/shared/vectors/vectors.S

include_bsp_HEADERS += ../../shared/vmeUniverse/vmeUniverse.h \
    ../../shared/vmeUniverse/vme_am_defs.h \
    ../../shared/vmeUniverse/VME.h \
    vme/VMEConfig.h \
    ../../powerpc/shared/motorola/motorola.h \
    ../../shared/vmeUniverse/vmeUniverseDMA.h\
    ../../shared/vmeUniverse/bspVmeDmaList.h\
    ../../shared/vmeUniverse/VMEDMA.h
# vme
libbsp_a_SOURCES += \
    ../../shared/vmeUniverse/bspVmeDmaList.c \
    ../../shared/vmeUniverse/vme_am_defs.h \
	../shared/vme/vmeconfig.c 

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
