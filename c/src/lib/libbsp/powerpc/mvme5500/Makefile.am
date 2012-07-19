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
nodist_include_HEADERS += ../../shared/include/coverhd.h

noinst_PROGRAMS =

###
dist_project_lib_DATA += ../shared/startup/linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# UNUSED
EXTRA_DIST = startup/bootpstuff.c

# startup
libbsp_a_SOURCES += startup/bspstart.c \
    ../../powerpc/shared/startup/pgtbl_setup.c startup/pgtbl_activate.c \
    ../../powerpc/shared/startup/pretaskinghook.c \
    ../../powerpc/shared/startup/zerobss.c \
    ../../powerpc/shared/startup/bspgetworkarea.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bootcard.c \
    ../../shared/bsppredriverhook.c startup/bspclean.c \
    ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/gnatinstallhandler.c startup/bspreset.c \
    ../../powerpc/shared/startup/probeMemEnd.c

# pclock
libbsp_a_SOURCES += ../../powerpc/shared/clock/p_clock.c

include_bsp_HEADERS = ../../powerpc/shared/console/uart.h
include_bsp_HEADERS += ../../powerpc/shared/console/consoleIo.h
# console
libbsp_a_SOURCES += ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c

include_bsp_HEADERS += pci/gtpcireg.h
include_bsp_HEADERS += ../../powerpc/shared/pci/pci.h
# pci
libbsp_a_SOURCES += pci/pci.c pci/pci_interface.c pci/detect_host_bridge.c \
    pci/pcifinddevice.c

include_bsp_HEADERS += irq/irq.h
# irq
libbsp_a_SOURCES += irq/irq_init.c irq/BSP_irq.c

nodist_include_HEADERS += ../../shared/tod.h
# tod
libbsp_a_SOURCES += ../../shared/tod.c tod/todcfg.c

include_bsp_HEADERS += vectors/bspException.h
include_bsp_HEADERS += \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h

# vectors
libbsp_a_SOURCES += vectors/exceptionhandler.c \
    ../../powerpc/shared/start/vectors_entry.S

include_bsp_HEADERS += GT64260/bspMvme5500.h GT64260/gtreg.h \
    GT64260/GT64260TWSI.h GT64260/VPD.h
# GT64260
libbsp_a_SOURCES += GT64260/GT64260TWSI.c GT64260/MVME5500I2C.c

include_bsp_HEADERS += ../../shared/vmeUniverse/VME.h vme/VMEConfig.h \
	../../shared/vmeUniverse/vmeUniverse.h \
	../../shared/vmeUniverse/vmeUniverseDMA.h\
	../../shared/vmeUniverse/bspVmeDmaList.h\
	../../shared/vmeUniverse/VMEDMA.h \
	../../shared/vmeUniverse/vme_am_defs.h
# vme
libbsp_a_SOURCES += ../../shared/vmeUniverse/vmeUniverse.c\
	../shared/vme/vmeconfig.c ../../shared/vmeUniverse/bspVmeDmaList.c\
	../shared/vme/vme_universe.c

if HAS_NETWORKING
include_bsp_HEADERS += network/if_100MHz/GT64260eth.h \
    network/if_100MHz/GT64260ethreg.h network/if_1GHz/if_wmreg.h \
    network/if_1GHz/pcireg.h

network_CPPFLAGS = -D_KERNEL -D__BSD_VISIBLE
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/if_100MHz/GT64260eth.c \
    network/if_1GHz/if_wm.c network/if_1GHz/pci_map.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = rtems_crti.$(OBJEXT)

noinst_PROGRAMS += mvme5500start.$(OBJEXT)
mvme5500start___OBJEXT__SOURCES = ../shared/start/preload.S start/start.S
mvme5500start___OBJEXT__CPPFLAGS = $(AM_CPPFLAGS)
mvme5500start___OBJEXT__LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += mvme5500start.$(OBJEXT)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel\
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/altivec.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
