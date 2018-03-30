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

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base
dist_project_lib_DATA += startup/linkcmds.br_uid
dist_project_lib_DATA += startup/linkcmds.hsc_cm01
dist_project_lib_DATA += startup/linkcmds.mpc8309som
dist_project_lib_DATA += startup/linkcmds.mpc8313erdb
dist_project_lib_DATA += startup/linkcmds.mpc8349eamds
dist_project_lib_DATA += startup/linkcmds.mpc83xx

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += \
	../../shared/bootcard.c \
	../../shared/getentropy-cpucounter.c \
	../../shared/bsppredriverhook.c \
	../../shared/sbrk.c \
	../../shared/gnatinstallhandler.c \
	../shared/src/tictac.c \
	startup/cpuinit.c \
	../shared/startup/bspidle.c \
	startup/bspstart.c \
	../../shared/bspclean.c \
	startup/bspreset.c \
	startup/bsprestart.c \
	../../shared/bspgetworkarea.c \
        ../../shared/src/bsp-uboot-board-info.c \
        ../shared/uboot_getenv.c

# clock
libbsp_a_SOURCES += ../shared/clock/clock.c

# irq
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c

# console
libbsp_a_SOURCES += ../../shared/console-termios-init.c
libbsp_a_SOURCES += ../../shared/console-termios.c
libbsp_a_SOURCES += console/console-config.c

# bsp_i2c
libbsp_a_SOURCES += i2c/i2c_init.c
# bsp_spi
libbsp_a_SOURCES += spi/spi_init.c

libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/bat.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/mmuAsm.S
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/pte121.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/gen83xx/dev/gtm.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/gen83xx/dev/mpc83xx_i2cdrv.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/gen83xx/dev/mpc83xx_spidrv.c

if HAS_NETWORKING
libbsp_a_SOURCES += network/network.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/net/tsec.c
endif


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions.am
include $(srcdir)/../../../../../../bsps/powerpc/gen83xx/headers.am
