##
#
# @file makefile.am
#
# @brief Makefile of LibBSP for the TMS570 boards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

# ----------------------------
# ------  Headers
# ----------------------------

# ----------------------------
# ------  Data
# ----------------------------

EXTRA_DIST += ../shared/start/start.S
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

noinst_LIBRARIES = libbsp.a

libbsp_a_SOURCES =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
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
libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

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

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/tms570/headers.am
