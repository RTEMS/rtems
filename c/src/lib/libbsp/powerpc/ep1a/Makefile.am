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

noinst_PROGRAMS =

include_bspdir = $(includedir)/bsp

###
dist_project_lib_DATA += startup/linkcmds

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/main.c ../../shared/bsppost.c ../../shared/bsplibc.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bspclean.c \
    ../../shared/gnatinstallhandler.c \
    ../../powerpc/shared/startup/pgtbl_setup.c \
    ../../powerpc/shared/startup/pgtbl_activate.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
noinst_PROGRAMS += pclock.rel
pclock_rel_SOURCES = ../../powerpc/shared/clock/p_clock.c
pclock_rel_CPPFLAGS = $(AM_CPPFLAGS)
pclock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
include_bsp_HEADERS = ../../powerpc/shared/console/uart.h \
    ../../powerpc/shared/motorola/motorola.h \
    ../../powerpc/shared/residual/residual.h \
    ../../powerpc/shared/residual/pnp.h \
    ../../powerpc/shared/console/consoleIo.h console/rsPMCQ1.h

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c console/ns16550cfg.c \
    console/mc68360_scc.c console/rsPMCQ1.c console/alloc360.c \
    console/init68360.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
###
include_bsp_HEADERS += ../../powerpc/shared/openpic/openpic.h

noinst_PROGRAMS += openpic.rel
openpic_rel_SOURCES = ../../powerpc/shared/openpic/openpic.h \
    ../../powerpc/shared/openpic/openpic.c

openpic_rel_CPPFLAGS = $(AM_CPPFLAGS)
openpic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
include_bsp_HEADERS += ../../powerpc/shared/pci/pci.h

noinst_PROGRAMS += pci.rel
pci_rel_SOURCES = pci/no_host_bridge.c ../../powerpc/shared/pci/pci.c \
    ../../powerpc/shared/pci/pcifinddevice.c
pci_rel_CPPFLAGS = $(AM_CPPFLAGS)
pci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
include_bsp_HEADERS += ../../powerpc/shared/irq/irq.h

noinst_PROGRAMS += irq.rel
irq_rel_SOURCES = irq/irq_init.c irq/irq.c ../../powerpc/shared/irq/i8259.c \
    ../../powerpc/shared/irq/irq_asm.S
irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

##
include_bsp_HEADERS += ../../powerpc/shared/vectors/vectors.h

noinst_PROGRAMS += vectors.rel
vectors_rel_SOURCES = ../../powerpc/shared/vectors/vectors_init.c \
    ../../powerpc/shared/vectors/vectors.S
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

##
include_bsp_HEADERS += ../../shared/vmeUniverse/vmeUniverse.h \
    ../../powerpc/shared/vme/VMEConfig.h ../../powerpc/shared/vme/VME.h \
    ../../shared/vmeUniverse/vme_am_defs.h

noinst_PROGRAMS += vme.rel
vme_rel_SOURCES = ../../shared/vmeUniverse/vmeUniverse.c vme/vmeconfig.c \
    ../../shared/vmeUniverse/vme_am_defs.h
vme_rel_CPPFLAGS = $(AM_CPPFLAGS)
vme_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

##

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = pclock.rel console.rel irq.rel openpic.rel pci.rel \
    vectors.rel startup.rel vme.rel

libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/exceptions.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
