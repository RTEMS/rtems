ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp
include_bestcommdir = $(include_bspdir)/bestcomm
include_bestcomm_includedir = $(include_bestcommdir)/include
include_bestcomm_include_mgt5200dir = $(include_bestcomm_includedir)/mgt5200
include_bestcomm_task_apidir = $(include_bestcommdir)/task_api

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h include/i2cdrv.h \
    include/i2c.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += ../../shared/include/coverhd.h

nodist_include_HEADERS += ../../shared/tod.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

libbspstart_a_SOURCES += ../../powerpc/shared/start/rtems_crti.S
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds.base
project_lib_DATA += startup/linkcmds
EXTRA_DIST = startup/linkcmds.brs5l
EXTRA_DIST += startup/linkcmds.icecube
EXTRA_DIST += startup/linkcmds.pm520_cr825
EXTRA_DIST += startup/linkcmds.pm520_ze30
EXTRA_DIST += startup/linkcmds.dp2

noinst_LIBRARIES += libbsp.a
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
include_bestcomm_HEADERS = bestcomm/bestcomm_priv.h \
	bestcomm/dma_image.h \
	bestcomm/dma_image.capi.h \
	bestcomm/bestcomm_api.h \
	bestcomm/bestcomm_glue.h
include_bestcomm_include_HEADERS = bestcomm/include/ppctypes.h \
	bestcomm/include/mgt5200/sdma.h \
	bestcomm/include/mgt5200/mgt5200.h
include_bestcomm_include_mgt5200_HEADERS = bestcomm/include/mgt5200/mgt5200.h \
	bestcomm/include/mgt5200/sdma.h
include_bestcomm_task_api_HEADERS = bestcomm/task_api/tasksetup_general.h \
	bestcomm/task_api/tasksetup_bdtable.h \
	bestcomm/task_api/bestcomm_cntrl.h \
	bestcomm/task_api/bestcomm_api_mem.h

# clock
# clock
libbsp_a_SOURCES += ../shared/clock/clock.c
# console
libbsp_a_SOURCES += console/console.c
# i2c
libbsp_a_SOURCES += i2c/i2c.c i2c/i2cdrv.c i2c/mpc5200mbus.c i2c/mpc5200mbus.h
# ide
libbsp_a_SOURCES += ide/idecfg.c ide/pcmcia_ide.c ide/pcmcia_ide.h

include_bsp_HEADERS = ../../shared/include/irq-generic.h \
	../../shared/include/irq-info.h \
	../../shared/include/utility.h \
	../../shared/include/bootcard.h \
	../../shared/include/u-boot.h \
	../shared/include/u-boot-board-info.h \
	include/irq.h \
	include/i2cdrv.h \
	include/i2c.h \
	include/mpc5200.h \
	include/mscan-base.h \
	include/u-boot-config.h \
	mscan/mscan.h

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

include_bsp_HEADERS += nvram/nvram.h
# nvram
libbsp_a_SOURCES += nvram/nvram.c nvram/nvram.h nvram/m93cxx.h

include_bsp_HEADERS += slicetimer/slicetimer.h
# slicetimer
libbsp_a_SOURCES += slicetimer/slicetimer.c slicetimer/slicetimer.h

# tod
libbsp_a_SOURCES += tod/todcfg.c tod/pcf8563.c tod/pcf8563.h \
    ../../shared/tod.c

# startup
libbsp_a_SOURCES += ../../shared/bootcard.c \
	../../shared/bsplibc.c \
	../../shared/bsppost.c \
	../../shared/bsppretaskinghook.c \
	../../shared/bsppredriverhook.c \
	../../shared/gnatinstallhandler.c \
	../../shared/sbrk.c \
	../../shared/src/bsp-uboot-board-info.c \
	../shared/showbats.c \
	../shared/uboot_dump_bdinfo.c \
	../shared/uboot_getenv.c \
	../../shared/bspclean.c \
	startup/bspreset.c \
	../../shared/bspgetworkarea.c \
	../shared/startup/bspidle.c \
	../shared/src/memcpy.c \
	startup/bspstart.c \
	startup/cpuinit.c  \
	startup/uboot_support.c

if HAS_NETWORKING
libbsp_a_SOURCES += network_5200/network.c
endif

libbsp_a_LIBADD =  ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
	../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
	../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
	../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
	../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
