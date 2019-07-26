##
#
# @file makefile.am
#
# @brief Makefile of LibBSP for the TMS570 boards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/arm/tms570/start/bsp_specs

# ----------------------------
# ------  Headers
# ----------------------------

# ----------------------------
# ------  Data
# ----------------------------

start.$(OBJEXT): ../../../../../../bsps/arm/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/arm/tms570/start/linkcmds.tms570ls3137_hdk
dist_project_lib_DATA += ../../../../../../bsps/arm/tms570/start/linkcmds.tms570ls3137_hdk_intram
dist_project_lib_DATA += ../../../../../../bsps/arm/tms570/start/linkcmds.tms570ls3137_hdk_sdram
dist_project_lib_DATA += ../../../../../../bsps/arm/tms570/start/linkcmds.tms570ls3137_hdk_with_loader

# ----------------------------
# ------  LibBSP
# ----------------------------

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-exception-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-ttb-entries.c

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/bspstart.c

# POM
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/tms570-pom.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/console/printk-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/console/tms570-sci.c

# Clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/arm/tms570/clock/clock.c

# RTC

# GPIO
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/pinmux.c

# Benchmark Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-cpucounter.c

# Misc
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/cpucounter/cpucounterread.c

# Watchdog

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

if TMS570_USE_HWINIT_STARTUP
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/tms570_sys_core.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/fail_notification.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/init_emif_sdram.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/init_esm.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/init_pinmux.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/init_system.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/tms570_selftest.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/tms570_selftest_par_can.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/tms570_selftest_par_mibspi.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/tms570_selftest_par_std.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/tms570_selftest_parity.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/tms570_tcram_tests.c

# Start hooks
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/bspstarthooks-hwinit.c
else
# Start hooks
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/tms570/start/bspstarthooks.c
endif

# ----------------------------
# ------  Special Rules
# ----------------------------


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/tms570/headers.am
