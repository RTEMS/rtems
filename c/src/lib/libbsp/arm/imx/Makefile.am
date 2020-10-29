ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/arm/imx/start/bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  Data                                                       #
###############################################################################

start.$(OBJEXT): ../../../../../../bsps/arm/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/arm/imx/start/linkcmds

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bsp-fdt.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-exception-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-ttb-entries.c

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/imx/start/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/imx/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/imx/start/ccm.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/pins/imx_iomux.c
if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/arm-a9mpcore-smp.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/imx/start/bspsmp.c
endif

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/irq/arm-gicv2.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/irq/irq-arm-gicvx-aarch32.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/imx/console/console-config.c

# Clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/clock/arm-generic-timer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/arm-generic-timer-aarch32.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-v7ar-disable-data.S

# GPIO
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/pins/imx-gpio.c

# I2C
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/imx/i2c/imx-i2c.c

# SPI
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/imx/spi/imx-ecspi.c

# Start hooks
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/imx/start/bspstarthooks.c

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/imx/headers.am
