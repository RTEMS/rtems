ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
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

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# bestcomm
libbsp_a_SOURCES += bestcomm/include/ppctypes.h \
    bestcomm/include/mgt5200/mgt5200.h bestcomm/include/mgt5200/sdma.h \
    bestcomm/bestcomm_api.c bestcomm/bestcomm_api.h bestcomm/bestcomm_glue.c \
    bestcomm/bestcomm_glue.h bestcomm/dma_image.c bestcomm/dma_image.capi.h \
    bestcomm/dma_image.h bestcomm/dma_image.reloc.c bestcomm/load_task.c \
    bestcomm/tasksetup_ata.c bestcomm/tasksetup_bdtable.c \
    bestcomm/tasksetup_crc16_dp_0.c bestcomm/tasksetup_crc16_dp_1.c \
    bestcomm/tasksetup_fec_rx_bd.c bestcomm/tasksetup_fec_tx_bd.c \
    bestcomm/tasksetup_gen_dp_0.c bestcomm/tasksetup_gen_dp_1.c \
    bestcomm/tasksetup_gen_dp_2.c bestcomm/tasksetup_gen_dp_3.c \
    bestcomm/tasksetup_gen_dp_bd_0.c bestcomm/tasksetup_gen_dp_bd_1.c \
    bestcomm/tasksetup_gen_rx_bd.c bestcomm/tasksetup_gen_tx_bd.c \
    bestcomm/tasksetup_lpc.c bestcomm/tasksetup_pci_rx.c \
    bestcomm/tasksetup_pci_tx.c bestcomm/bestcomm_priv.h \
    bestcomm/task_api/bestcomm_api_mem.h bestcomm/task_api/bestcomm_cntrl.h \
    bestcomm/task_api/tasksetup_bdtable.h \
    bestcomm/task_api/tasksetup_general.h
# clock
# clock
libbsp_a_SOURCES += ../shared/clock/clock.c
# console
libbsp_a_SOURCES += console/console.c
# i2c
libbsp_a_SOURCES += i2c/i2c.c i2c/i2cdrv.c i2c/mpc5200mbus.c i2c/mpc5200mbus.h
# ide
libbsp_a_SOURCES += ide/idecfg.c ide/pcmcia_ide.c ide/pcmcia_ide.h
libbsp_a_SOURCES += ide/ata.c
libbsp_a_SOURCES += ide/ata-instance.c
libbsp_a_SOURCES += ide/ata-dma-pio-single.c

# irq
libbsp_a_SOURCES += include/irq.h \
	irq/irq.c \
	../../shared/src/irq-generic.c \
	../../shared/src/irq-legacy.c \
	../../shared/src/irq-info.c \
	../../shared/src/irq-shell.c \
	../../shared/src/irq-server.c

# mscan
libbsp_a_SOURCES += mscan/mscan.c mscan/mscan-base.c mscan/mscan.h mscan/mscan_int.h

# nvram
libbsp_a_SOURCES += nvram/nvram.c nvram/nvram.h nvram/m93cxx.h

# slicetimer
libbsp_a_SOURCES += slicetimer/slicetimer.c slicetimer/slicetimer.h

# tod
libbsp_a_SOURCES += tod/todcfg.c tod/pcf8563.c tod/pcf8563.h \
    ../../shared/tod.c

# startup
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/bsp-uboot-board-info.c
libbsp_a_SOURCES += ../shared/showbats.c
libbsp_a_SOURCES += ../shared/src/memcpy.c
libbsp_a_SOURCES += ../shared/startup/bspidle.c
libbsp_a_SOURCES += ../shared/uboot_dump_bdinfo.c
libbsp_a_SOURCES += ../shared/uboot_getenv.c
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/cpuinit.c 
libbsp_a_SOURCES += startup/uboot_support.c
libbsp_a_SOURCES += startup/bestcomm.c

if HAS_NETWORKING
libbsp_a_SOURCES += network_5200/network.c
endif

libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/bat.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/mmuAsm.S
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/pte121.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions.am
include $(srcdir)/../../../../../../bsps/powerpc/gen5200/headers.am
