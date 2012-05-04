ACLOCAL_AMFLAGS = -I ../../../../aclocal

CLEANFILES      =
DISTCLEANFILES  =
noinst_PROGRAMS =
EXTRA_DIST      = 

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs
project_lib_DATA      = 

#include
include_HEADERS = include/bsp.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
nodist_include_HEADERS += ../../shared/include/coverhd.h
nodist_include_HEADERS += ../../shared/include/tm27.h
DISTCLEANFILES += include/bspopts.h

include_bsp_HEADERS =

#start
EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)


EXTRA_DIST += ../../powerpc/shared/start/preload.S
preload.$(OBJEXT): ../../powerpc/shared/start/preload.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

EXTRA_DIST += ../../powerpc/shared/start/vectors_entry.S
vectors_entry.$(OBJEXT): ../../powerpc/shared/start/vectors_entry.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

EXTRA_DIST += ../../powerpc/shared/start/start.S
start.$(OBJEXT): ../../powerpc/shared/start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

motld_start.$(OBJEXT):	preload.$(OBJEXT) vectors_entry.$(OBJEXT) start.$(OBJEXT)
	$(LD) -o $@ -r $^

project_lib_DATA += motld_start.$(OBJEXT)


#startup

dist_project_lib_DATA += ../shared/startup/linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

libbsp_a_SOURCES += startup/bspstart.c \
    ../shared/motorola/vpd.c startup/reboot.c startup/i2c_init.c \
    ../../powerpc/shared/startup/panic.c \
    ../../powerpc/shared/startup/bspgetworkarea.c \
    ../../powerpc/shared/startup/probeMemEnd.c \
    ../../powerpc/shared/startup/pretaskinghook.c \
    ../../powerpc/shared/startup/zerobss.c \
    ../../powerpc/shared/startup/pgtbl_setup.c \
    ../../powerpc/shared/startup/pgtbl_activate.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bootcard.c \
    startup/bspclean.c ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/gnatinstallhandler.c

include_bsp_HEADERS += ../shared/motorola/vpd.h

#pclock
libbsp_a_SOURCES += ../../powerpc/shared/clock/p_clock.c

#console
include_bsp_HEADERS += ../../powerpc/shared/console/consoleIo.h
include_bsp_HEADERS += ../../powerpc/shared/console/uart.h

libbsp_a_SOURCES +=  \
    ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c \
    ../../powerpc/shared/console/consoleIo.h \
    ../../powerpc/shared/console/uart.h

#irq
include_bsp_HEADERS += irq/irq.h

libbsp_a_SOURCES += irq/irq_init.c irq/discovery_pic.c 

#marvell
include_bsp_HEADERS += marvell/gtreg.h marvell/gtintrreg.h \
    marvell/gti2creg.h marvell/gti2c_busdrv.h marvell/gt_timer.h \
    marvell/gtpcireg.h

libbsp_a_SOURCES += marvell/discovery.c marvell/gti2c.c marvell/gt_timer.c

#flash
include_bsp_HEADERS += ../shared/flash/flashPgm.h
include_bsp_HEADERS += ../shared/flash/flashPgmPvt.h

libbsp_a_SOURCES += ../shared/flash/flash.c \
    ../shared/flash/intelFlash.c \
    flash/flashcfg.c

#pci
include_bsp_HEADERS += ../../powerpc/shared/pci/pci.h

libbsp_a_SOURCES += ../../powerpc/shared/pci/pci.c \
    pci/gt_pci_init.c pci/pci_io_remap.c pci/motload_fixup.c \
    ../../powerpc/shared/pci/pcifinddevice.c

#vectors
include_bsp_HEADERS += ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/vectors.h
include_bsp_HEADERS += ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h

#vme
include_bsp_HEADERS += vme/VMEConfig.h \
    ../../shared/vmeUniverse/vmeUniverse.h \
    ../../shared/vmeUniverse/vmeUniverseDMA.h \
    ../../shared/vmeUniverse/vme_am_defs.h \
    ../../shared/vmeUniverse/vmeTsi148.h \
    ../../shared/vmeUniverse/vmeTsi148DMA.h \
    ../../shared/vmeUniverse/bspVmeDmaList.h \
    ../../shared/vmeUniverse/VME.h \
    ../../shared/vmeUniverse/VMEDMA.h

libbsp_a_SOURCES += ../shared/vme/vmeconfig.c \
    ../shared/vme/vme_universe.c \
    ../../shared/vmeUniverse/vmeUniverse.c \
    ../../shared/vmeUniverse/vmeTsi148.c \
    ../../shared/vmeUniverse/bspVmeDmaList.c

#network
if HAS_NETWORKING
include_bsp_HEADERS += network/support/early_enet_link_status.h \
    network/support/bsp_bsdnet_attach.h

noinst_PROGRAMS += network_support.rel
network_support_rel_SOURCES = network/support/early_link_status.c \
    network/support/bsp_attach.c
network_support_rel_CPPFLAGS = $(AM_CPPFLAGS)
network_support_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS += network/if_mve/if_mve_pub.h

noinst_PROGRAMS += network_if_mve_tmp.rel
network_if_mve_tmp_rel_SOURCES  = network/if_mve/mv643xx_eth.c
network_if_mve_tmp_rel_CPPFLAGS = $(AM_CPPFLAGS) -DDISABLE_DETACHING
network_if_mve_tmp_rel_CFLAGS   = $(AM_CFLAGS)
network_if_mve_tmp_rel_LDFLAGS  = $(RTEMS_RELLDFLAGS)

# remove all unneccessary global symbols to avoid name clashes
# with BSD stuff;
network_if_mve.rel: network_if_mve_tmp.rel
	$(OBJCOPY) -G rtems_mve_attach -G rtems_mve_early_link_check_ops \
                   -G BSP_mve_ack_irqs -G BSP_mve_disable_irqs \
                   -G BSP_mve_enable_irqs -G BSP_mve_init_hw \
                   -G BSP_mve_ack_irq_mask -G BSP_mve_disable_irq_mask \
                   -G BSP_mve_enable_irq_mask -G BSP_mve_setup_1 \
                   -G BSP_mve_read_eaddr -G BSP_mve_send_buf \
                   -G BSP_mve_send_buf_raw \
                   -G BSP_mve_setup -G BSP_mve_stop_hw \
                   -G BSP_mve_swipe_rx -G BSP_mve_swipe_tx \
                   -G BSP_mve_detach -G BSP_mve_media_ioctl \
                   -G BSP_mve_get_tid \
                   -G BSP_mve_dump_stats -G BSP_mve_ack_link_chg \
                   -G BSP_mve_mcast_filter_clear \
                   -G BSP_mve_mcast_filter_accept_all \
                   -G BSP_mve_mcast_filter_accept_add \
                   -G BSP_mve_mcast_filter_accept_del \
                   -G mveth_serial_ctrl_config_val \
	           $^ $@

include_bsp_HEADERS += network/if_gfe/if_gfe_pub.h

noinst_PROGRAMS += network_if_gfe_tmp.rel
network_if_gfe_tmp_rel_SOURCES = network/if_gfe/if_gfe.c network/if_gfe/if_gfe_rtems.c
network_if_gfe_tmp_rel_CPPFLAGS = $(AM_CPPFLAGS) \
    -I$(srcdir)/network/porting -I$(srcdir)/network/if_gfe
network_if_gfe_tmp_rel_CFLAGS   = $(AM_CFLAGS)
network_if_gfe_tmp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# remove all unneccessary global symbols to avoid name clashes
# with BSD stuff;
network_if_gfe.rel: network_if_gfe_tmp.rel
	$(OBJCOPY) -G rtems_gfe_attach -G net_driver_ticks_per_sec \
                   -G rtems_gfe_setup -G rtems_gfe_early_link_check_ops \
                   $^ $@


include_bsp_HEADERS += network/if_em/if_em_pub.h

noinst_PROGRAMS += network_if_em_tmp.rel
network_if_em_tmp_rel_SOURCES = network/if_em/if_em.c \
    network/if_em/if_em_hw.c \
    network/if_em/if_em_rtems.c
network_if_em_tmp_rel_CPPFLAGS = $(AM_CPPFLAGS) \
    -I$(srcdir)/network/porting -I$(srcdir)/network/if_em
network_if_em_tmp_rel_CFLAGS   = $(AM_CFLAGS)
network_if_em_tmp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

network_if_em.rel: network_if_em_tmp.rel
	$(OBJCOPY) -G rtems_em_attach -G net_driver_ticks_per_sec \
                   -G rtems_em_pci_setup -G rtems_em_early_link_check_ops \
                   $^ $@
endif

# tod
nodist_include_HEADERS += ../../shared/tod.h

libbsp_a_SOURCES += ../../shared/tod.c tod/todcfg.c

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/altivec.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network_support.rel \
    network_if_mve.rel network_if_gfe.rel network_if_em.rel
endif

EXTRA_DIST += README LICENSE ChangeLog

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
