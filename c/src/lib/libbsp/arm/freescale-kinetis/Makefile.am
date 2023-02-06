##
#
# @file makefile.am
#
# @brief Makefile of LibBSP for the FREESCALE_KINETIS boards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/arm/freescale-kinetis/start/bsp_specs

# ----------------------------
# ------  Headers
# ----------------------------

# ----------------------------
# ------  Data
# ----------------------------

start.$(OBJEXT): ../../../../../../bsps/arm/freescale-kinetis/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/arm/freescale-kinetis/start/linkcmds.dk_k60
dist_project_lib_DATA += ../../../../../../bsps/arm/freescale-kinetis/start/linkcmds.dk_k60_psram

# ----------------------------
# ------  LibBSP
# ----------------------------

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/gpio/gpio-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/start/bspreset.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/irq/irq-armv7m.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/irq/irq-dispatch-armv7m.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/console/uart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/console/termios_callback.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/console/console_config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/console/console_early.c

# Clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/clock-armv7m.c

# FlexBus
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/flexbus/flexbus.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/flexbus/flexbus_glcd.c
if FREESCALE_KINETIS_USING_PSRAM
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/flexbus/flexbus_psram.c
endif

# SPI
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/spi/spi.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/spi/spi-config.c

# Flash
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/spi/spi-flash-device.c
#librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/spi/spiflash.c

# SD Card
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/spi/sd-card-init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/spi/spi_sd_drv.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/spi/sd-blkdev-ioctl.c

# GPIO
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/gpio/gpio_config_k60.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/gpio/gpio_interfaces_k60.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/gpio/kinetis_gpio.c

# Benchmark Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-cpucounter.c

# Misc
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/font/font.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/picture/picture.c

# RTC
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/rtc/rtc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/rtc/rtc-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

# Start hooks
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/freescale-kinetis/start/bspstarthooks.c

# ----------------------------
# ------  Special Rules
# ----------------------------


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/freescale-kinetis/headers.am
