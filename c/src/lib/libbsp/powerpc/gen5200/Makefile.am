##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h include/i2cdrv.h  include/u-boot.h \
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
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds.pm520

noinst_PROGRAMS += bestcomm.rel
bestcomm_rel_SOURCES = ./bestcomm/bestcomm_api.c\
                       ./bestcomm/bestcomm_glue.c\
                       ./bestcomm/dma_image.c\
                       ./bestcomm/dma_image.reloc.c\
                       ./bestcomm/load_task.c\
                       ./bestcomm/tasksetup_ata.c\
                       ./bestcomm/tasksetup_bdtable.c\
                       ./bestcomm/tasksetup_crc16_dp_0.c\
                       ./bestcomm/tasksetup_crc16_dp_1.c\
                       ./bestcomm/tasksetup_fec_rx_bd.c\
                       ./bestcomm/tasksetup_fec_tx_bd.c\
                       ./bestcomm/tasksetup_gen_dp_0.c\
                       ./bestcomm/tasksetup_gen_dp_1.c\
                       ./bestcomm/tasksetup_gen_dp_2.c\
                       ./bestcomm/tasksetup_gen_dp_3.c\
                       ./bestcomm/tasksetup_gen_dp_bd_0.c\
                       ./bestcomm/tasksetup_gen_dp_bd_1.c\
                       ./bestcomm/tasksetup_gen_rx_bd.c\
                       ./bestcomm/tasksetup_gen_tx_bd.c\
                       ./bestcomm/tasksetup_lpc.c\
                       ./bestcomm/tasksetup_pci_rx.c\
                       ./bestcomm/tasksetup_pci_tx.c
bestcomm_rel_CPPFLAGS = $(AM_CPPFLAGS)
bestcomm_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/clock.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += i2c.rel
i2c_rel_SOURCES = ./i2c/i2c.c           \
                  ./i2c/i2cdrv.c        \
                  ./i2c/mpc5200mbus.c
i2c_rel_CPPFLAGS = $(AM_CPPFLAGS)
i2c_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += ide.rel
ide_rel_SOURCES = ./ide/idecfg.c ./ide/pcmcia_ide.c
ide_rel_CPPFLAGS = $(AM_CPPFLAGS)
ide_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS = irq/irq.h include/i2cdrv.h include/mpc5200.h include/i2c.h include/u-boot.h

noinst_PROGRAMS += irq.rel
irq_rel_SOURCES = irq/irq.c irq/irq_init.c irq/irq.h irq/irq_asm.S
irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)


noinst_PROGRAMS += mscan.rel
mscan_rel_SOURCES = ./mscan/mscan.c
mscan_rel_CPPFLAGS = $(AM_CPPFLAGS)
mscan_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
include_bsp_HEADERS += mscan/mscan.h

noinst_PROGRAMS += nvram.rel
nvram_rel_SOURCES = ./nvram/nvram.c
nvram_rel_CPPFLAGS = $(AM_CPPFLAGS)
nvram_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += slicetimer.rel
slicetimer_rel_SOURCES = ./slicetimer/slicetimer.c
slicetimer_rel_CPPFLAGS = $(AM_CPPFLAGS)
slicetimer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += tod.rel
tod_rel_SOURCES = ./tod/todcfg.c ./tod/pcf8563.c ../../shared/tod.c
tod_rel_CPPFLAGS = $(AM_CPPFLAGS)
tod_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS += vectors/vectors.h

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/main.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c startup/cpuinit.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += vectors.rel
vectors_rel_SOURCES = vectors/vectors_init.c vectors/vectors.h \
    vectors/vectors.S
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
network_CPPFLAGS += -D__BSD_VISIBLE
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network_5200/network.c 
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = bestcomm.rel clock.rel console.rel i2c.rel ide.rel irq.rel \
                  mscan.rel nvram.rel slicetimer.rel tod.rel startup.rel     \
                  vectors.rel
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/exceptions.rel

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST += times

PREINSTALL_DIRS =
PREINSTALL_FILES =
TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/$(dirstamp): $(PROJECT_INCLUDE)/$(dirstamp)
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/bestcomm/$(dirstamp): $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp/bestcomm
	@: > $(PROJECT_INCLUDE)/bsp/bestcomm/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/bsp/bestcomm/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/bestcomm/include/$(dirstamp): $(PROJECT_INCLUDE)/bsp/bestcomm/$(dirstamp)
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp/bestcomm/include
	@: > $(PROJECT_INCLUDE)/bsp/bestcomm/include/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/bsp/bestcomm/include/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/bestcomm/include/mgt5200/$(dirstamp): $(PROJECT_INCLUDE)/bsp/bestcomm/include/$(dirstamp)
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp/bestcomm/include/mgt5200
	@: > $(PROJECT_INCLUDE)/bsp/bestcomm/include/mgt5200/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/bsp/bestcomm/include/mgt5200/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/bestcomm/task_api/$(dirstamp): $(PROJECT_INCLUDE)/bsp/bestcomm/$(dirstamp)
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp/bestcomm/task_api
	@: > $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/$(dirstamp)

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/bsp_specs: bsp_specs $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/bsp_specs
PREINSTALL_FILES += $(PROJECT_LIB)/bsp_specs

$(PROJECT_INCLUDE)/bsp.h: include/bsp.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp.h

$(PROJECT_INCLUDE)/i2c.h: include/i2c.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/i2c.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/i2c.h

$(PROJECT_INCLUDE)/i2cdrv.h: include/i2cdrv.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/i2cdrv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/i2cdrv.h

$(PROJECT_INCLUDE)/u-boot.h: include/u-boot.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/u-boot.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/u-boot.h

$(PROJECT_INCLUDE)/mpc5200.h: include/mpc5200.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc5200.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc5200.h

$(PROJECT_INCLUDE)/tm27.h: include/tm27.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tm27.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tm27.h

$(PROJECT_INCLUDE)/bspopts.h: include/bspopts.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bspopts.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bspopts.h

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_LIB)/start.$(OBJEXT): start.$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start.$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start.$(OBJEXT)

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
PREINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_INCLUDE)/bsp/irq.h: irq/irq.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

$(PROJECT_INCLUDE)/bsp/vectors.h: vectors/vectors.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vectors.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vectors.h

$(PROJECT_INCLUDE)/bsp/bestcomm/ppctypes.h: bestcomm/include/ppctypes.h $(PROJECT_INCLUDE)/bsp/bestcomm/include/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/include/ppctypes.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/ppctypes.h

$(PROJECT_INCLUDE)/bsp/bestcomm/bestcomm_api.h: bestcomm/bestcomm_api.h $(PROJECT_INCLUDE)/bsp/bestcomm/include/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/bestcomm_api.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/bestcomm_api.h

$(PROJECT_INCLUDE)/bsp/bestcomm/bestcomm_glue.h: bestcomm/bestcomm_glue.h $(PROJECT_INCLUDE)/bsp/bestcomm/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/bestcomm_glue.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/bestcomm_glue.h

$(PROJECT_INCLUDE)/bsp/bestcomm/bestcomm_priv.h: bestcomm/bestcomm_priv.h $(PROJECT_INCLUDE)/bsp/bestcomm/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/bestcomm_priv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/bestcomm_priv.h

$(PROJECT_INCLUDE)/bsp/bestcomm/dma_image.capi.h: bestcomm/dma_image.capi.h $(PROJECT_INCLUDE)/bsp/bestcomm/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/dma_image.capi.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/dma_image.capi.h

$(PROJECT_INCLUDE)/bsp/bestcomm/dma_image.h: bestcomm/dma_image.h $(PROJECT_INCLUDE)/bsp/bestcomm/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/dma_image.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/dma_image.h

$(PROJECT_INCLUDE)/bsp/bestcomm/include/mgt5200/mgt5200.h: bestcomm/include/mgt5200/mgt5200.h $(PROJECT_INCLUDE)/bsp/bestcomm/include/mgt5200/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/include/mgt5200/mgt5200.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/include/mgt5200/mgt5200.h

$(PROJECT_INCLUDE)/bsp/bestcomm/include/mgt5200/sdma.h: bestcomm/include/mgt5200/sdma.h $(PROJECT_INCLUDE)/bsp/bestcomm/include/mgt5200/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/include/mgt5200/sdma.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/include/mgt5200/sdma.h

$(PROJECT_INCLUDE)/bsp/bestcomm/include/ppctypes.h: bestcomm/include/ppctypes.h $(PROJECT_INCLUDE)/bsp/bestcomm/include/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/include/ppctypes.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/include/ppctypes.h

$(PROJECT_INCLUDE)/bsp/bestcomm/task_api/bestcomm_api_mem.h: bestcomm/task_api/bestcomm_api_mem.h $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/bestcomm_api_mem.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/bestcomm_api_mem.h

$(PROJECT_INCLUDE)/bsp/bestcomm/task_api/bestcomm_cntrl.h: bestcomm/task_api/bestcomm_cntrl.h $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/bestcomm_cntrl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/bestcomm_cntrl.h

$(PROJECT_INCLUDE)/bsp/bestcomm/task_api/tasksetup_bdtable.h: bestcomm/task_api/tasksetup_bdtable.h $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/tasksetup_bdtable.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/tasksetup_bdtable.h

$(PROJECT_INCLUDE)/bsp/bestcomm/task_api/tasksetup_general.h: bestcomm/task_api/tasksetup_general.h $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/tasksetup_general.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bestcomm/task_api/tasksetup_general.h

$(PROJECT_INCLUDE)/bsp/pcmcia_ide.h: ide/pcmcia_ide.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pcmcia_ide.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pcmcia_ide.h

$(PROJECT_INCLUDE)/bsp/mscan.h: mscan/mscan.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/mscan.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/mscan.h

$(PROJECT_INCLUDE)/bsp/m93cxx.h: nvram/m93cxx.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/m93cxx.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/m93cxx.h

$(PROJECT_INCLUDE)/bsp/nvram.h: nvram/nvram.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/nvram.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/nvram.h

$(PROJECT_INCLUDE)/bsp/slicetimer.h: slicetimer/slicetimer.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/slicetimer.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/slicetimer.h

$(PROJECT_INCLUDE)/bsp/pcf8563.h: tod/pcf8563.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pcf8563.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pcf8563.h


CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES += $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
