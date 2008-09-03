##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

libcpudir = ../../../libcpu/@RTEMS_CPU@

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h include/i2cdrv.h include/u-boot.h \
    include/i2c.h
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

dist_project_lib_DATA += startup/linkcmds \
	startup/linkcmds.base \
	startup/linkcmds.brs5l \
	startup/linkcmds.icecube \
	startup/linkcmds.pm520

bestcomm_SOURCES = bestcomm/include/ppctypes.h \
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

clock_SOURCES = clock/clock.c
console_SOURCES = console/console.c
i2c_SOURCES = i2c/i2c.c i2c/i2cdrv.c i2c/mpc5200mbus.c i2c/mpc5200mbus.h
ide_SOURCES = ide/idecfg.c ide/pcmcia_ide.c ide/pcmcia_ide.h

include_bsp_HEADERS = $(libcpudir)/new-exceptions/bspsupport/vectors.h \
	$(libcpudir)/new-exceptions/bspsupport/ppc_exc_bspsupp.h \
	../../shared/include/irq-generic.h \
	include/irq-config.h \
	include/irq.h \
	include/i2cdrv.h \
	include/i2c.h \
	include/mpc5200.h \
	include/mscan-base.h \
	include/u-boot.h \
	mscan/mscan.h

irq_SOURCES = include/irq.h \
	include/irq-config.h \
	irq/irq.c \
	../../shared/src/irq-generic.c \
	../../shared/src/irq-legacy.c

mscan_SOURCES = mscan/mscan.c mscan/mscan-base.c mscan/mscan.h mscan/mscan_int.h

include_bsp_HEADERS += nvram/nvram.h
nvram_SOURCES = nvram/nvram.c nvram/nvram.h nvram/m93cxx.h

include_bsp_HEADERS += slicetimer/slicetimer.h
slicetimer_SOURCES = slicetimer/slicetimer.c slicetimer/slicetimer.h

tod_SOURCES = tod/todcfg.c tod/pcf8563.c tod/pcf8563.h \
    ../../shared/tod.c

startup_SOURCES = ../../shared/bootcard.c \
	../../shared/bsplibc.c \
	../../shared/bsppost.c \
	../../shared/bsppretaskinghook.c \
	../../shared/gnatinstallhandler.c \
	../../shared/sbrk.c \
	../shared/showbats.c \
	../shared/uboot_dump_bdinfo.c \
	../shared/uboot_getenv.c \
	startup/bspclean.c \
	startup/bspstart.c \
	startup/cpuinit.c  \
	startup/uboot_support.c

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
    $(slicetimer_SOURCES) $(tod_SOURCES) $(startup_SOURCES) $(bestcomm_SOURCES)

libbsp_a_LIBADD =  $(libcpudir)/shared/cpuIdent.rel \
	$(libcpudir)/shared/cache.rel \
	$(libcpudir)/shared/stack.rel \
	$(libcpudir)/@exceptions@/rtems-cpu.rel \
	$(libcpudir)/@exceptions@/raw_exception.rel \
	$(libcpudir)/@exceptions@/exc_bspsupport.rel \
	$(libcpudir)/mpc6xx/mmu.rel \
	$(libcpudir)/mpc6xx/timer.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
