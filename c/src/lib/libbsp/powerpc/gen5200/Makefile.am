ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/powerpc/gen5200/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.brs5l
dist_project_lib_DATA += startup/linkcmds.brs6l
dist_project_lib_DATA += startup/linkcmds.dp2
dist_project_lib_DATA += startup/linkcmds.gen5200_base
dist_project_lib_DATA += startup/linkcmds.icecube
dist_project_lib_DATA += startup/linkcmds.pm520_cr825
dist_project_lib_DATA += startup/linkcmds.pm520_ze30

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# bestcomm
librtemsbsp_a_SOURCES += bestcomm/bestcomm_api.c
librtemsbsp_a_SOURCES += bestcomm/bestcomm_glue.c
librtemsbsp_a_SOURCES += bestcomm/dma_image.c
librtemsbsp_a_SOURCES += bestcomm/dma_image.reloc.c
librtemsbsp_a_SOURCES += bestcomm/load_task.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_ata.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_bdtable.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_crc16_dp_0.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_crc16_dp_1.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_fec_rx_bd.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_fec_tx_bd.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_gen_dp_0.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_gen_dp_1.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_gen_dp_2.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_gen_dp_3.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_gen_dp_bd_0.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_gen_dp_bd_1.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_gen_rx_bd.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_gen_tx_bd.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_lpc.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_pci_rx.c
librtemsbsp_a_SOURCES += bestcomm/tasksetup_pci_tx.c
# clock
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/shared/clock/clock.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/gen5200/console/console.c
# i2c
librtemsbsp_a_SOURCES += i2c/i2c.c
librtemsbsp_a_SOURCES += i2c/i2cdrv.c
librtemsbsp_a_SOURCES += i2c/mpc5200mbus.c
# ide
librtemsbsp_a_SOURCES += ide/idecfg.c
librtemsbsp_a_SOURCES += ide/pcmcia_ide.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/gen5200/dev/mpc5200-ata.c
librtemsbsp_a_SOURCES += ide/ata-instance.c
librtemsbsp_a_SOURCES += ide/ata-dma-pio-single.c

# irq
librtemsbsp_a_SOURCES += irq/irq.c

# mscan
librtemsbsp_a_SOURCES += mscan/mscan.c
librtemsbsp_a_SOURCES += mscan/mscan-base.c

# nvram
librtemsbsp_a_SOURCES += nvram/nvram.c

# slicetimer
librtemsbsp_a_SOURCES += slicetimer/slicetimer.c

# tod
librtemsbsp_a_SOURCES += tod/todcfg.c
librtemsbsp_a_SOURCES += tod/pcf8563.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bsp-uboot-board-info.c
librtemsbsp_a_SOURCES += ../shared/showbats.c
librtemsbsp_a_SOURCES += ../shared/src/memcpy.c
librtemsbsp_a_SOURCES += ../shared/startup/bspidle.c
librtemsbsp_a_SOURCES += ../shared/uboot_dump_bdinfo.c
librtemsbsp_a_SOURCES += ../shared/uboot_getenv.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/cpuinit.c 
librtemsbsp_a_SOURCES += startup/uboot_support.c
librtemsbsp_a_SOURCES += startup/bestcomm.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network_5200/network.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/btimer/btimer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/bat.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/mmuAsm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/pte121.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/gen5200/headers.am
