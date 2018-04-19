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

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base
dist_project_lib_DATA += startup/linkcmds.br_uid
dist_project_lib_DATA += startup/linkcmds.hsc_cm01
dist_project_lib_DATA += startup/linkcmds.mpc8309som
dist_project_lib_DATA += startup/linkcmds.mpc8313erdb
dist_project_lib_DATA += startup/linkcmds.mpc8349eamds
dist_project_lib_DATA += startup/linkcmds.mpc83xx

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../shared/src/tictac.c
librtemsbsp_a_SOURCES += startup/cpuinit.c
librtemsbsp_a_SOURCES += ../shared/startup/bspidle.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += startup/bsprestart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bsp-uboot-board-info.c
librtemsbsp_a_SOURCES += ../shared/uboot_getenv.c

# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/shared/clock/clock.c

# irq
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += irq/irq.c

# console
librtemsbsp_a_SOURCES += ../../shared/console-termios-init.c
librtemsbsp_a_SOURCES += ../../shared/console-termios.c
librtemsbsp_a_SOURCES += console/console-config.c

# bsp_i2c
librtemsbsp_a_SOURCES += i2c/i2c_init.c
# bsp_spi
librtemsbsp_a_SOURCES += spi/spi_init.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/bat.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/mmuAsm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/pte121.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/gen83xx/dev/gtm.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/gen83xx/dev/mpc83xx_i2cdrv.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/gen83xx/dev/mpc83xx_spidrv.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/network.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/net/tsec.c
endif


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/gen83xx/headers.am
