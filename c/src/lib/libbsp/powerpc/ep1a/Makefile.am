ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

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
libbsp_a_SOURCES += startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/bsppost.c ../../shared/bsppredriverhook.c \
    ../../powerpc/shared/startup/bspgetworkarea.c ../../shared/bsplibc.c \
    ../../powerpc/shared/startup/sbrk.c \
    ../../shared/bspclean.c ../../shared/gnatinstallhandler.c \
    ../../powerpc/shared/startup/pgtbl_setup.c \
    ../../powerpc/shared/startup/pgtbl_activate.c \
    ../../powerpc/shared/showbats.c

# pclock
libbsp_a_SOURCES += ../../powerpc/shared/clock/p_clock.c

include_bsp_HEADERS = ../../powerpc/shared/console/uart.h \
    ../../powerpc/shared/motorola/motorola.h \
    ../../powerpc/shared/residual/residual.h \
    ../../powerpc/shared/residual/pnp.h \
    ../../powerpc/shared/console/consoleIo.h console/rsPMCQ1.h \
    ../../shared/console_private.h

# console
libbsp_a_SOURCES += console/ns16550cfg.c \
    console/mc68360_scc.c console/rsPMCQ1.c console/alloc360.c \
    console/init68360.c console/config.c console/printk_support.c \
    ../../shared/console.c ../../shared/console_select.c \
    ../../shared/console_read.c ../../shared/console_write.c \
    ../../shared/console_control.c
 
include_bsp_HEADERS += ../../powerpc/shared/openpic/openpic.h
# openpic
libbsp_a_SOURCES += ../../powerpc/shared/openpic/openpic.h \
    ../../powerpc/shared/openpic/openpic.c

include_bsp_HEADERS += ../../powerpc/shared/pci/pci.h
# pci
libbsp_a_SOURCES += pci/no_host_bridge.c ../../powerpc/shared/pci/pci.c \
    ../../powerpc/shared/pci/pcifinddevice.c

include_bsp_HEADERS += ../../powerpc/shared/irq/irq.h \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h
# irq
libbsp_a_SOURCES += irq/irq_init.c irq/openpic_xxx_irq.c ../../powerpc/shared/irq/i8259.c

include_bsp_HEADERS += ../../shared/vmeUniverse/vmeUniverse.h \
    vme/VMEConfig.h \
    ../../shared/vmeUniverse/vme_am_defs.h \
    ../../shared/vmeUniverse/VME.h \
    ../../shared/vmeUniverse/vmeUniverseDMA.h\
    ../../shared/vmeUniverse/bspVmeDmaList.h\
    ../../shared/vmeUniverse/VMEDMA.h

# vme
libbsp_a_SOURCES += ../../shared/vmeUniverse/vmeUniverse.c \
    ../../shared/vmeUniverse/bspVmeDmaList.c \
    ../shared/vme/vmeconfig.c \
    ../shared/vme/vme_universe.c \
    ../../shared/vmeUniverse/vme_am_defs.h

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
