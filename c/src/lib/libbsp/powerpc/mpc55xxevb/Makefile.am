ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs



noinst_PROGRAMS =

start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../shared/start/rtems_crti.S
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

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c
librtemsbsp_a_SOURCES += ../shared/src/bsp-start-zero.S
librtemsbsp_a_SOURCES += ../shared/src/memcpy.c
librtemsbsp_a_SOURCES += ../shared/src/tictac.c
librtemsbsp_a_SOURCES += ../shared/src/ppc-exc-handler-table.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/bspgetworkarea.c
librtemsbsp_a_SOURCES += startup/exc-vector-base.S
librtemsbsp_a_SOURCES += startup/get-system-clock.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += startup/restart.c
librtemsbsp_a_SOURCES += startup/idle-thread.c
librtemsbsp_a_SOURCES += startup/start-config-clock.c
librtemsbsp_a_SOURCES += startup/start-config-ebi.c
librtemsbsp_a_SOURCES += startup/start-config-ebi-cs.c
librtemsbsp_a_SOURCES += startup/start-config-ebi-cs-cal.c
librtemsbsp_a_SOURCES += startup/start-config-mmu.c
librtemsbsp_a_SOURCES += startup/start-config-mmu-early.c
librtemsbsp_a_SOURCES += startup/start-config-siu-pcr.c
librtemsbsp_a_SOURCES += startup/start-prologue.c
librtemsbsp_a_SOURCES += startup/start-early.c
librtemsbsp_a_SOURCES += startup/start-cache.S
librtemsbsp_a_SOURCES += startup/start-clock.c
librtemsbsp_a_SOURCES += startup/start-flash.S
librtemsbsp_a_SOURCES += startup/start-watchdog.c

# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/mpc55xxevb/clock/clock-config.c

# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/console/console-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/console/console-esci.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/console/console-generic.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/console/console-linflex.c

# irq_generic
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c

# Misc
librtemsbsp_a_SOURCES += startup/sd-card-init.c

# I2C
librtemsbsp_a_SOURCES += i2c/i2c_init.c

# Network
if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/smsc9218i.c
librtemsbsp_a_SOURCES += network/if_smc.c
endif

# BSP library

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/btimer/btimer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/gen83xx/dev/mpc83xx_i2cdrv.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/dev/dspi.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/start/copy.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/start/edma.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/start/emios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/start/flash_support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/start/irq.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc55xxevb/start/siu.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/mpc55xxevb/headers.am
