ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

CLEANFILES      =
DISTCLEANFILES  =
noinst_PROGRAMS =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

project_lib_DATA      = 

#include
DISTCLEANFILES += include/bspopts.h

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

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.share

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

libbsp_a_SOURCES += startup/bspstart.c \
    ../shared/motorola/vpd.c startup/bspreset.c startup/i2c_init.c \
    ../../powerpc/shared/startup/bspgetworkarea.c \
    ../../powerpc/shared/startup/probeMemEnd.c \
    ../../powerpc/shared/startup/bsppredriverhook.c \
    ../../powerpc/shared/startup/zerobss.c \
    ../../powerpc/shared/startup/pgtbl_setup.c \
    ../../powerpc/shared/startup/pgtbl_activate.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    startup/bspclean.c \
    ../../shared/gnatinstallhandler.c

#pclock
libbsp_a_SOURCES += ../../powerpc/shared/clock/p_clock.c

#console
libbsp_a_SOURCES +=  \
    ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c \
    ../../powerpc/shared/console/consoleIo.h \
    ../../powerpc/shared/console/uart.h

#irq
libbsp_a_SOURCES += irq/irq_init.c irq/discovery_pic.c 

#marvell
libbsp_a_SOURCES += marvell/discovery.c marvell/gti2c.c marvell/gt_timer.c

#flash
libbsp_a_SOURCES += ../shared/flash/flash.c \
    ../shared/flash/intelFlash.c \
    flash/flashcfg.c

#pci
libbsp_a_SOURCES += ../../powerpc/shared/pci/pci.c \
    pci/gt_pci_init.c pci/pci_io_remap.c pci/motload_fixup.c \
    ../../powerpc/shared/pci/pcifinddevice.c

#vme
libbsp_a_SOURCES += ../shared/vme/vmeconfig.c \
    ../shared/vme/vme_universe.c \
    ../../shared/vmeUniverse/vmeUniverse.c \
    ../../shared/vmeUniverse/vmeTsi148.c \
    ../../shared/vmeUniverse/bspVmeDmaList.c

#network
if HAS_NETWORKING
noinst_PROGRAMS += network_support.rel
network_support_rel_SOURCES = network/support/early_link_status.c \
    network/support/bsp_attach.c
network_support_rel_CPPFLAGS = $(AM_CPPFLAGS)
network_support_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

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

libbsp_a_LIBADD = network_support.rel \
    network_if_mve.rel network_if_gfe.rel network_if_em.rel
endif

# tod
libbsp_a_SOURCES += ../../shared/tod.c tod/todcfg.c

libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup_asm.S
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/clock-ppc-dec.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/bat.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/mmuAsm.S
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/pte121.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/ppc-irq-legacy.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/ppc-dec-timer.c

EXTRA_DIST += README LICENSE

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions.am
include $(srcdir)/../../../../../../bsps/powerpc/beatnik/headers.am
