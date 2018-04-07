ACLOCAL_AMFLAGS = -I ../../../../aclocal

CLEANFILES      =
noinst_PROGRAMS =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

project_lib_DATA      = 

#include

#start
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)


preload.$(OBJEXT): ../../powerpc/shared/start/preload.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

vectors_entry.$(OBJEXT): ../../powerpc/shared/start/vectors_entry.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

start.$(OBJEXT): ../../powerpc/shared/start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

motld_start.$(OBJEXT):	preload.$(OBJEXT) vectors_entry.$(OBJEXT) start.$(OBJEXT)
	$(LD) -o $@ -r $^

project_lib_DATA += motld_start.$(OBJEXT)


#startup

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.share

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

librtemsbsp_a_SOURCES += startup/bspstart.c \
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
librtemsbsp_a_SOURCES += ../../powerpc/shared/clock/p_clock.c

#console
librtemsbsp_a_SOURCES +=  \
    ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c \
    ../../powerpc/shared/console/consoleIo.h \
    ../../powerpc/shared/console/uart.h

#irq
librtemsbsp_a_SOURCES += irq/irq_init.c irq/discovery_pic.c 

#marvell
librtemsbsp_a_SOURCES += marvell/discovery.c marvell/gti2c.c marvell/gt_timer.c

#flash
librtemsbsp_a_SOURCES += ../shared/flash/flash.c \
    ../shared/flash/intelFlash.c \
    flash/flashcfg.c

#pci
librtemsbsp_a_SOURCES += ../../powerpc/shared/pci/pci.c \
    pci/gt_pci_init.c pci/pci_io_remap.c pci/motload_fixup.c \
    ../../powerpc/shared/pci/pcifinddevice.c

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
librtemsbsp_a_SOURCES += ../../shared/tod.c tod/todcfg.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup_asm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/clock-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
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
