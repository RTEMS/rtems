##
#
# @file makefile.am
#
# @brief Makefile of LibBSP for the TMS570 boards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp
include_libcpudir = $(includedir)/libcpu

include_bsp_ti_herculesdir = $(includedir)/bsp/ti_herc

dist_project_lib_DATA = bsp_specs

# ----------------------------
# ------  Headers
# ----------------------------

include_HEADERS = include/bsp.h

nodist_include_HEADERS = ../../shared/include/coverhd.h
nodist_include_HEADERS += include/bspopts.h

nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h

include_bsp_HEADERS =
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/stackalloc.h
include_bsp_HEADERS += ../../shared/include/uart-output-char.h
include_bsp_HEADERS += ../shared/include/start.h
include_bsp_HEADERS += include/tms570.h
include_bsp_HEADERS += include/tms570-sci.h
include_bsp_HEADERS += include/irq.h
include_bsp_HEADERS += include/tms570-rti.h
include_bsp_HEADERS += include/tms570-vim.h
include_bsp_HEADERS += include/tms570-pom.h
include_bsp_HEADERS += include/tms570-sci-driver.h
include_bsp_HEADERS += include/system-clocks.h
include_bsp_HEADERS += include/tms570-pinmux.h

include_bsp_HEADERS += include/tms570-pins.h
include_bsp_HEADERS += include/tms570ls3137zwt-pins.h

include_libcpu_HEADERS = ../../../libcpu/arm/shared/include/arm-cp15.h

include_bsp_ti_hercules_HEADERS = include/ti_herc/reg_adc.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_ccmsr.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_crc.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_dcan.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_dcc.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_dma.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_dmm.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_efuse.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_emacc.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_emacm.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_emif.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_esm.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_flash.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_flex_ray.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_gio.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_htu.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_i2c.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_iomm.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_lin.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_mdio.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_n2het.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_pbist.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_pll.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_pmm.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_rti.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_rtp.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_sci.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_tcr.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_tcram.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_vim.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_pom.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_spi.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_stc.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_sys.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_sys2.h
include_bsp_ti_hercules_HEADERS += include/ti_herc/reg_pcr.h

include_HEADERS += ../../shared/include/tm27.h

# ----------------------------
# ------  Data
# ----------------------------

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = ../shared/start/start.S

project_lib_DATA = start.$(OBJEXT)
project_lib_DATA += startup/linkcmds

EXTRA_DIST =
EXTRA_DIST += startup/linkcmds.tms570ls3137_hdk
EXTRA_DIST += startup/linkcmds.tms570ls3137_hdk_with_loader
EXTRA_DIST += startup/linkcmds.tms570ls3137_hdk_sdram
EXTRA_DIST += startup/linkcmds.tms570ls3137_hdk_intram

# ----------------------------
# ------  LibBSP
# ----------------------------

noinst_LIBRARIES += libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS =
libbsp_a_LIBADD =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
libbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c

# Startup
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += ../shared/startup/bsp-start-init-registers.S
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += startup/bspstart.c

# POM
libbsp_a_SOURCES += pom/tms570-pom.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c

# Console
libbsp_a_SOURCES += ../../shared/console-termios.c
libbsp_a_SOURCES += console/printk-support.c
libbsp_a_SOURCES += console/tms570-sci.c

# Clock
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
libbsp_a_SOURCES += clock/clock.c

# RTC

# GPIO
libbsp_a_SOURCES += pinmux/pinmux.c

# Benchmark Timer
libbsp_a_SOURCES += ../../shared/timercpucounter.c

# Misc
libbsp_a_SOURCES += misc/cpucounterread.c

# Watchdog

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../../libcpu/arm/shared/include/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../../../libcpu/arm/shared/include

if TMS570_USE_HWINIT_STARTUP
libbsp_a_SOURCES += hwinit/tms570_sys_core.S
libbsp_a_SOURCES += hwinit/fail_notification.c
libbsp_a_SOURCES += hwinit/init_emif_sdram.c
libbsp_a_SOURCES += hwinit/init_esm.c
libbsp_a_SOURCES += hwinit/init_pinmux.c
libbsp_a_SOURCES += hwinit/init_system.c
libbsp_a_SOURCES += hwinit/tms570_selftest.c
libbsp_a_SOURCES += hwinit/tms570_selftest_par_can.c
libbsp_a_SOURCES += hwinit/tms570_selftest_par_mibspi.c
libbsp_a_SOURCES += hwinit/tms570_selftest_par_std.c
libbsp_a_SOURCES += hwinit/tms570_selftest_parity.c
libbsp_a_SOURCES += hwinit/tms570_tcram_tests.c

include_bsp_HEADERS += hwinit/tms570_selftest_parity.h
include_bsp_HEADERS += hwinit/tms570_hwinit.h
include_bsp_HEADERS += hwinit/tms570_selftest.h

# Start hooks
libbsp_a_SOURCES += hwinit/bspstarthooks-hwinit.c
else
# Start hooks
libbsp_a_SOURCES += startup/bspstarthooks.c
endif

# ----------------------------
# ------  Special Rules
# ----------------------------

DISTCLEANFILES = include/bspopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
