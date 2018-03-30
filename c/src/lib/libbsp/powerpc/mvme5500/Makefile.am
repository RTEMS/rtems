ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# UNUSED

# startup
libbsp_a_SOURCES += startup/bspstart.c \
    ../../powerpc/shared/startup/pgtbl_setup.c startup/pgtbl_activate.c \
    ../../powerpc/shared/startup/bsppredriverhook.c \
    ../../powerpc/shared/startup/zerobss.c \
    ../../powerpc/shared/startup/bspgetworkarea.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    startup/bspclean.c \
    ../../shared/gnatinstallhandler.c startup/bspreset.c \
    ../../powerpc/shared/startup/probeMemEnd.c

# pclock
libbsp_a_SOURCES += ../../powerpc/shared/clock/p_clock.c

# console
libbsp_a_SOURCES += ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c

# pci
libbsp_a_SOURCES += pci/pci.c
libbsp_a_SOURCES += pci/pci_interface.c
libbsp_a_SOURCES += pci/detect_host_bridge.c
libbsp_a_SOURCES += ../../shared/pci/pci_find_device.c

# irq
libbsp_a_SOURCES += irq/irq_init.c irq/BSP_irq.c

# tod
libbsp_a_SOURCES += ../../shared/tod.c tod/todcfg.c

# vectors
libbsp_a_SOURCES += vectors/exceptionhandler.c \
    ../../powerpc/shared/start/vectors_entry.S

# GT64260
libbsp_a_SOURCES += GT64260/GT64260TWSI.c GT64260/MVME5500I2C.c

# vme
libbsp_a_SOURCES += ../../shared/vmeUniverse/vmeUniverse.c\
	../shared/vme/vmeconfig.c ../../shared/vmeUniverse/bspVmeDmaList.c\
	../shared/vme/vme_universe.c

if HAS_NETWORKING
network_CPPFLAGS = -D_KERNEL -D__BSD_VISIBLE
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/if_100MHz/GT64260eth.c \
    network/if_1GHz/if_wm.c network/if_1GHz/pci_map.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
libbsp_a_LIBADD = network.rel
endif

rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = rtems_crti.$(OBJEXT)

noinst_PROGRAMS += mvme5500start.$(OBJEXT)
mvme5500start___OBJEXT__SOURCES = ../shared/start/preload.S start/start.S
mvme5500start___OBJEXT__CPPFLAGS = $(AM_CPPFLAGS)
mvme5500start___OBJEXT__LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += mvme5500start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.share

libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup_asm.S
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/clock-ppc-dec.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/bat.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/mmuAsm.S
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/pte121.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/ppc-irq-legacy.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions.am
include $(srcdir)/../../../../../../bsps/powerpc/mvme5500/headers.am
