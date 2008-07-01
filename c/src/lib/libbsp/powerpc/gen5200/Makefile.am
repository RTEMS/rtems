##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h include/i2cdrv.h include/u-boot.h \
    include/i2c.h include/mpc5200.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_bspdir = $(includedir)/bsp

include_HEADERS += include/coverhd.h

nodist_include_HEADERS += ../../shared/tod.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds
dist_project_lib_DATA += startup/linkcmds.brs5l
dist_project_lib_DATA += startup/linkcmds.icecube
dist_project_lib_DATA += startup/linkcmds.pm520

noinst_PROGRAMS += bestcomm.rel
bestcomm_rel_SOURCES = bestcomm/include/ppctypes.h \
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
bestcomm_rel_CPPFLAGS = $(AM_CPPFLAGS) -DMPC5200_BAPI_LIBC_HEADERS
bestcomm_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

clock_SOURCES = clock/clock.c
console_SOURCES = console/console.c
i2c_SOURCES = i2c/i2c.c i2c/i2cdrv.c i2c/mpc5200mbus.c i2c/mpc5200mbus.h
ide_SOURCES = ide/idecfg.c ide/pcmcia_ide.c ide/pcmcia_ide.h

include_bsp_HEADERS = irq/irq.h include/i2cdrv.h include/mpc5200.h \
    include/i2c.h include/u-boot.h
irq_SOURCES = irq/irq.c irq/irq_init.c irq/irq.h irq/irq_asm.S
mscan_SOURCES = mscan/mscan.c mscan/mscan.h mscan/mscan_int.h

include_bsp_HEADERS += nvram/nvram.h
nvram_SOURCES = nvram/nvram.c nvram/nvram.h nvram/m93cxx.h

include_bsp_HEADERS += slicetimer/slicetimer.h
slicetimer_SOURCES = slicetimer/slicetimer.c slicetimer/slicetimer.h

tod_SOURCES = tod/todcfg.c tod/pcf8563.c tod/pcf8563.h \
    ../../shared/tod.c

include_bsp_HEADERS += vectors/vectors.h

startup_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/sbrk.c ../shared/uboot_dump_bdinfo.c \
    ../../shared/gnatinstallhandler.c startup/cpuinit.c  \
    ../shared/showbats.c startup/uboot_support.c ../shared/uboot_getenv.c

vectors_SOURCES = vectors/vectors_init.c vectors/vectors.h \
    vectors/vectors.S

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
network_CPPFLAGS += -D__BSD_VISIBLE
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network_5200/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(clock_SOURCES) $(console_SOURCES) $(i2c_SOURCES) \
    $(ide_SOURCES) $(irq_SOURCES) $(mscan_SOURCES) $(nvram_SOURCES) \
    $(slicetimer_SOURCES) $(tod_SOURCES) $(startup_SOURCES) \
    $(vectors_SOURCES)

libbsp_a_LIBADD = \
    bestcomm.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
