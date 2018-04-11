##
#
# @file makefile.am
#
# @brief Makefile of LibBSP for the TMS570 boards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

# ----------------------------
# ------  Headers
# ----------------------------

# ----------------------------
# ------  Data
# ----------------------------

start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.tms570ls3137_hdk
dist_project_lib_DATA += startup/linkcmds.tms570ls3137_hdk_intram
dist_project_lib_DATA += startup/linkcmds.tms570ls3137_hdk_sdram
dist_project_lib_DATA += startup/linkcmds.tms570ls3137_hdk_with_loader

# ----------------------------
# ------  LibBSP
# ----------------------------

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Shared
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/src/stackalloc.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterdiff.c
librtemsbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
librtemsbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c

# Startup
librtemsbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../shared/startup/bsp-start-init-registers.S
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += startup/bspstart.c

# POM
librtemsbsp_a_SOURCES += pom/tms570-pom.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../shared/console-termios.c
librtemsbsp_a_SOURCES += console/printk-support.c
librtemsbsp_a_SOURCES += console/tms570-sci.c

# Clock
librtemsbsp_a_SOURCES += clock/clock.c

# RTC

# GPIO
librtemsbsp_a_SOURCES += pinmux/pinmux.c

# Benchmark Timer
librtemsbsp_a_SOURCES += ../../shared/timercpucounter.c

# Misc
librtemsbsp_a_SOURCES += misc/cpucounterread.c

# Watchdog

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

if TMS570_USE_HWINIT_STARTUP
librtemsbsp_a_SOURCES += hwinit/tms570_sys_core.S
librtemsbsp_a_SOURCES += hwinit/fail_notification.c
librtemsbsp_a_SOURCES += hwinit/init_emif_sdram.c
librtemsbsp_a_SOURCES += hwinit/init_esm.c
librtemsbsp_a_SOURCES += hwinit/init_pinmux.c
librtemsbsp_a_SOURCES += hwinit/init_system.c
librtemsbsp_a_SOURCES += hwinit/tms570_selftest.c
librtemsbsp_a_SOURCES += hwinit/tms570_selftest_par_can.c
librtemsbsp_a_SOURCES += hwinit/tms570_selftest_par_mibspi.c
librtemsbsp_a_SOURCES += hwinit/tms570_selftest_par_std.c
librtemsbsp_a_SOURCES += hwinit/tms570_selftest_parity.c
librtemsbsp_a_SOURCES += hwinit/tms570_tcram_tests.c

# Start hooks
librtemsbsp_a_SOURCES += hwinit/bspstarthooks-hwinit.c
else
# Start hooks
librtemsbsp_a_SOURCES += startup/bspstarthooks.c
endif

# ----------------------------
# ------  Special Rules
# ----------------------------


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/tms570/headers.am
