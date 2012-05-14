ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += ../../shared/include/coverhd.h
include_HEADERS += ../../shared/tod.h

project_lib_DATA =

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<

EXTRA_DIST += ../shared/start/preload.S
preload.$(OBJEXT): ../shared/start/preload.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

EXTRA_DIST += ../shared/start/vectors_entry.S
vectors_entry.$(OBJEXT): ../shared/start/vectors_entry.S
	$(CPPASCOMPILE) -o $@ -c $<

motld_start.$(OBJEXT):	preload.$(OBJEXT) vectors_entry.$(OBJEXT) \
    start.$(OBJEXT)
	$(LD) -o $@ -r $^

project_lib_DATA += motld_start.$(OBJEXT)

EXTRA_DIST += ../shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += ../shared/startup/linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += startup/bspstart.c \
    startup/misc.c ../../powerpc/shared/startup/bspgetworkarea.c \
    ../../powerpc/shared/startup/pretaskinghook.c \
    ../../powerpc/shared/startup/zerobss.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bootcard.c \
    ../../shared/bspclean.c ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/gnatinstallhandler.c
# tod
libbsp_a_SOURCES += ../../shared/tod.c tod/todcfg.c
# pclock
libbsp_a_SOURCES += ../../powerpc/shared/clock/p_clock.c

include_bsp_HEADERS = ../../powerpc/shared/console/uart.h
# console
libbsp_a_SOURCES += ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c \
    ../../powerpc/shared/console/consoleIo.h \
    ../../powerpc/shared/console/uart.h

include_bsp_HEADERS += irq/irq.h
# irq
libbsp_a_SOURCES += irq/irq_init.c \
    ../../powerpc/shared/irq/openpic_i8259_irq.c \
    ../../powerpc/shared/irq/irq.h

include_bsp_HEADERS += ../../powerpc/shared/openpic/openpic.h
# openpic
libbsp_a_SOURCES += ../../powerpc/shared/openpic/openpic.h \
    ../../powerpc/shared/openpic/openpic.c \
    ../../powerpc/shared/openpic/openpic.h

include_bsp_HEADERS += ../../powerpc/shared/pci/pci.h
# pci
libbsp_a_SOURCES += ../../powerpc/shared/pci/pci.c \
    pci/detect_host_bridge.c \
    ../../powerpc/shared/pci/generic_clear_hberrs.c \
    ../../powerpc/shared/pci/pcifinddevice.c ../../powerpc/shared/pci/pci.h

include_bsp_HEADERS += \
  ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h

include_bsp_HEADERS += vme/VMEConfig.h \
    ../../shared/vmeUniverse/vmeTsi148.h \
    ../../shared/vmeUniverse/vme_am_defs.h \
    ../../shared/vmeUniverse/VME.h \
    ../../shared/vmeUniverse/vmeTsi148DMA.h\
    ../../shared/vmeUniverse/bspVmeDmaList.h\
    ../../shared/vmeUniverse/VMEDMA.h

# vme
libbsp_a_SOURCES += ../../shared/vmeUniverse/vmeTsi148.c \
    ../../shared/vmeUniverse/bspVmeDmaList.c \
    ../../shared/vmeUniverse/vmeTsi148.h \
    ../../shared/vmeUniverse/vme_am_defs.h \
    ../../shared/vmeUniverse/VME.h \
    ../../powerpc/shared/vme/vmeconfig.c \
    ../../powerpc/shared/vme/vme_universe.c

include_bsp_HEADERS += ../shared/flash/flashPgm.h \
    ../shared/flash/flashPgmPvt.h
# flash
libbsp_a_SOURCES += ../shared/flash/flash.c \
    ../shared/flash/spansionFlash.c \
    flash/flashcfg.c

include_bsp_HEADERS += i2c/mpc8540_i2c_busdrv.h
# i2c
libbsp_a_SOURCES += i2c/mpc8540_i2c.c i2c/i2c_init.c

include_bsp_HEADERS += ../shared/motorola/vpd.h
# vpd
libbsp_a_SOURCES += ../shared/motorola/vpd.c

if HAS_NETWORKING
include_bsp_HEADERS += network/if_tsec_pub.h
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/tsec.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/e500/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/e500/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/e500/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

EXTRA_DIST += LICENSE ChangeLog README KNOWN_PROBLEMS

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
