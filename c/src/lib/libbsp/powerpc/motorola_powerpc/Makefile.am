##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . bootloader

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_bspdir = $(includedir)/bsp

include_HEADERS += ../../powerpc/shared/include/nvram.h
include_HEADERS += ../../shared/include/coverhd.h
include_HEADERS += ../../shared/tod.h

EXTRA_DIST = ../../powerpc/shared/start/start.S
start.$(OBJEXT): ../../powerpc/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/vectors_entry.S
vectors_entry.$(OBJEXT): ../../powerpc/shared/start/vectors_entry.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += vectors_entry.$(OBJEXT)


dist_project_lib_DATA += ../shared/startup/linkcmds

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = ../../powerpc/shared/startup/bspstart.c \
 	../../powerpc/shared/showbats.c \
 	../../powerpc/shared/startup/pretaskinghook.c \
        ../../shared/bsppredriverhook.c \
 	../../powerpc/shared/startup/zerobss.c \
    ../../powerpc/shared/startup/pgtbl_setup.c \
    ../../powerpc/shared/startup/pgtbl_activate.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bootcard.c \
    ../../shared/bspclean.c ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += tod.rel
tod_rel_SOURCES = ../../shared/tod.c ../../powerpc/shared/tod/todcfg.c
tod_rel_CPPFLAGS = $(AM_CPPFLAGS)
tod_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += pclock.rel
pclock_rel_SOURCES = ../../powerpc/shared/clock/p_clock.c
pclock_rel_CPPFLAGS = $(AM_CPPFLAGS)
pclock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS = ../../powerpc/shared/console/consoleIo.h
include_bsp_HEADERS += ../../powerpc/shared/console/uart.h

console_CPPFLAGS = -DSTATIC_LOG_ALLOC
noinst_PROGRAMS += console.rel
console_rel_SOURCES = ../../powerpc/shared/console/polled_io.c \
    ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c \
    ../../powerpc/shared/console/inch.c \
    ../../powerpc/shared/console/reboot.c \
    ../../powerpc/shared/console/consoleIo.h \
    ../../powerpc/shared/console/keyboard.h \
    ../../powerpc/shared/console/uart.h
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(console_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS += ../../powerpc/shared/irq/irq.h \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/vectors.h \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/ppc_exc_bspsupp.h

noinst_PROGRAMS += irq.rel
irq_rel_SOURCES = ../../powerpc/shared/irq/irq_init.c ../../powerpc/shared/irq/openpic_i8259_irq.c \
    ../../powerpc/shared/irq/i8259.c ../../powerpc/shared/irq/irq.h
irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS += ../../powerpc/shared/motorola/motorola.h

noinst_PROGRAMS += motorola.rel
motorola_rel_SOURCES = ../../powerpc/shared/motorola/motorola.h \
    ../../powerpc/shared/motorola/motorola.c
motorola_rel_CPPFLAGS = $(AM_CPPFLAGS)
motorola_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS += ../../powerpc/shared/openpic/openpic.h

noinst_PROGRAMS += openpic.rel
openpic_rel_SOURCES = ../../powerpc/shared/openpic/openpic.h \
    ../../powerpc/shared/openpic/openpic.c \
    ../../powerpc/shared/openpic/openpic.h

openpic_rel_CPPFLAGS = $(AM_CPPFLAGS)
openpic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS += ../../powerpc/shared/pci/pci.h

noinst_PROGRAMS += pci.rel
pci_rel_SOURCES = ../../powerpc/shared/pci/pci.c \
    ../../powerpc/shared/pci/detect_raven_bridge.c \
    ../../powerpc/shared/pci/generic_clear_hberrs.c \
    ../../powerpc/shared/pci/pcifinddevice.c ../../powerpc/shared/pci/pci.h
pci_rel_CPPFLAGS = $(AM_CPPFLAGS)
pci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS += ../../powerpc/shared/residual/residual.h \
    ../../powerpc/shared/residual/pnp.h

noinst_PROGRAMS += residual.rel
residual_rel_SOURCES = ../../powerpc/shared/residual/pnp.h \
    ../../powerpc/shared/residual/residual.h \
    ../../powerpc/shared/residual/residual.c
residual_rel_CPPFLAGS = $(AM_CPPFLAGS)
residual_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS += ../../shared/vmeUniverse/vmeUniverse.h \
    ../../shared/vmeUniverse/vme_am_defs.h \
    ../../shared/vmeUniverse/VME.h \
    ../../powerpc/shared/vme/VMEConfig.h \
    ../../shared/vmeUniverse/vmeUniverseDMA.h\
    ../../shared/vmeUniverse/bspVmeDmaList.h\
    ../../shared/vmeUniverse/VMEDMA.h

noinst_PROGRAMS += vme.rel
vme_rel_SOURCES = ../../shared/vmeUniverse/vmeUniverse.c \
    ../../shared/vmeUniverse/bspVmeDmaList.c \
    ../../shared/vmeUniverse/vmeUniverse.h \
    ../../shared/vmeUniverse/vme_am_defs.h \
    ../../shared/vmeUniverse/VME.h \
    ../../powerpc/shared/vme/vmeconfig.c \
    ../../powerpc/shared/vme/vme_universe.c \
    ../../powerpc/shared/vme/VMEConfig.h
vme_rel_CPPFLAGS = $(AM_CPPFLAGS)
vme_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup.rel pclock.rel console.rel irq.rel openpic.rel \
    pci.rel residual.rel motorola.rel tod.rel vme.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel

EXTRA_DIST += BOOTING README.mtx603e README.MVME2100 README.MVME2300 \
              README.MVME2400 \
    README.OTHERBOARDS README.mcp750 README.MVME2307 README.dec21140
EXTRA_DIST += times.mcp750 times.mvme2307

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
