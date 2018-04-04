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

# Link commands
project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base
dist_project_lib_DATA += startup/linkcmds.gwlcfm
dist_project_lib_DATA += startup/linkcmds.mpc5566evb
dist_project_lib_DATA += startup/linkcmds.mpc5566evb_spe
dist_project_lib_DATA += startup/linkcmds.mpc55xx
dist_project_lib_DATA += startup/linkcmds.mpc5643l_dpu
dist_project_lib_DATA += startup/linkcmds.mpc5643l_evb
dist_project_lib_DATA += startup/linkcmds.mpc5668g
dist_project_lib_DATA += startup/linkcmds.mpc5674f_ecu508
dist_project_lib_DATA += startup/linkcmds.mpc5674f_ecu508_app
dist_project_lib_DATA += startup/linkcmds.mpc5674f_ecu508_boot
dist_project_lib_DATA += startup/linkcmds.mpc5674fevb
dist_project_lib_DATA += startup/linkcmds.mpc5674fevb_spe
dist_project_lib_DATA += startup/linkcmds.mpc5674f_rsm6
dist_project_lib_DATA += startup/linkcmds.mpc5674f_rsm6_base
dist_project_lib_DATA += startup/linkcmds.phycore_mpc5554

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../shared/src/bsp-start-zero.S
libbsp_a_SOURCES += ../shared/src/memcpy.c
libbsp_a_SOURCES += ../shared/src/tictac.c
libbsp_a_SOURCES += ../shared/src/ppc-exc-handler-table.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/bspgetworkarea.c
libbsp_a_SOURCES += startup/exc-vector-base.S
libbsp_a_SOURCES += startup/get-system-clock.c
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += startup/restart.c
libbsp_a_SOURCES += startup/idle-thread.c
libbsp_a_SOURCES += startup/start-config-clock.c
libbsp_a_SOURCES += startup/start-config-ebi.c
libbsp_a_SOURCES += startup/start-config-ebi-cs.c
libbsp_a_SOURCES += startup/start-config-ebi-cs-cal.c
libbsp_a_SOURCES += startup/start-config-mmu.c
libbsp_a_SOURCES += startup/start-config-mmu-early.c
libbsp_a_SOURCES += startup/start-config-siu-pcr.c
libbsp_a_SOURCES += startup/start-prologue.c
libbsp_a_SOURCES += startup/start-early.c
libbsp_a_SOURCES += startup/start-cache.S
libbsp_a_SOURCES += startup/start-clock.c
libbsp_a_SOURCES += startup/start-flash.S
libbsp_a_SOURCES += startup/start-watchdog.c

# clock
libbsp_a_SOURCES += clock/clock-config.c

# console
libbsp_a_SOURCES += console/console-config.c
libbsp_a_SOURCES += console/console-esci.c
libbsp_a_SOURCES += console/console-generic.c
libbsp_a_SOURCES += console/console-linflex.c

# irq_generic
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c

# Misc
libbsp_a_SOURCES += startup/sd-card-init.c

# I2C
libbsp_a_SOURCES += i2c/i2c_init.c

# Network
if HAS_NETWORKING
libbsp_a_SOURCES += network/smsc9218i.c
libbsp_a_SOURCES += network/if_smc.c
endif

# BSP library

libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/gen83xx/dev/mpc83xx_i2cdrv.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/dev/dspi.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/start/copy.S
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/start/edma.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/start/emios.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/start/flash_support.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/start/irq.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/start/siu.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/mpc55xxevb/headers.am
