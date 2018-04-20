ACLOCAL_AMFLAGS = -I ../../../../aclocal

CLEANFILES      =
noinst_PROGRAMS =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

project_lib_DATA      = 

#include

#start
rtems_crti.$(OBJEXT): ../shared/start/rtems_crti.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)


preload.$(OBJEXT): ../shared/start/preload.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

vectors_entry.$(OBJEXT): ../shared/start/vectors_entry.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

motld_start.$(OBJEXT):	preload.$(OBJEXT) vectors_entry.$(OBJEXT) start.$(OBJEXT)
	$(LD) -o $@ -r $^

project_lib_DATA += motld_start.$(OBJEXT)


#startup

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.share

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += ../shared/motorola/vpd.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += startup/i2c_init.c
librtemsbsp_a_SOURCES += ../shared/startup/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../shared/startup/probeMemEnd.c
librtemsbsp_a_SOURCES += ../shared/startup/zerobss.c
librtemsbsp_a_SOURCES += ../shared/startup/pgtbl_setup.c
librtemsbsp_a_SOURCES += ../shared/startup/pgtbl_activate.c
librtemsbsp_a_SOURCES += ../shared/startup/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += startup/bspclean.c

#pclock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/shared/clock/p_clock.c

#console
librtemsbsp_a_SOURCES += ../shared/console/uart.c
librtemsbsp_a_SOURCES += ../shared/console/console.c

#irq
librtemsbsp_a_SOURCES += irq/irq_init.c
librtemsbsp_a_SOURCES += irq/discovery_pic.c 

#marvell
librtemsbsp_a_SOURCES += marvell/discovery.c
librtemsbsp_a_SOURCES += marvell/gti2c.c
librtemsbsp_a_SOURCES += marvell/gt_timer.c

#flash
librtemsbsp_a_SOURCES += ../shared/flash/flash.c
librtemsbsp_a_SOURCES += ../shared/flash/intelFlash.c
librtemsbsp_a_SOURCES += flash/flashcfg.c

#pci
librtemsbsp_a_SOURCES += ../shared/pci/pci.c
librtemsbsp_a_SOURCES += pci/gt_pci_init.c
librtemsbsp_a_SOURCES += pci/pci_io_remap.c
librtemsbsp_a_SOURCES += pci/motload_fixup.c
librtemsbsp_a_SOURCES += ../shared/pci/pcifinddevice.c

#network
if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/if_em/if_em.c
librtemsbsp_a_SOURCES += network/if_em/if_em_hw.c
librtemsbsp_a_SOURCES += network/if_em/if_em_rtems.c
librtemsbsp_a_SOURCES += network/if_gfe/if_gfe.c
librtemsbsp_a_SOURCES += network/if_gfe/if_gfe_rtems.c
librtemsbsp_a_SOURCES += network/if_mve/mv643xx_eth.c
librtemsbsp_a_SOURCES += network/support/bsp_attach.c
librtemsbsp_a_SOURCES += network/support/early_link_status.c
endif

# tod
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += tod/todcfg.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup_asm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/clock/clock-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/btimer/btimer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/bat.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/mmuAsm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/pte121.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/ppc-irq-legacy.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/vme-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/beatnik/headers.am
