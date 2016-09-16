include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/multilib.am

include_devdir = $(includedir)/dev
include_dev_HEADERS =

include_dev_i2cdir = $(includedir)/dev/i2c
include_dev_i2c_HEADERS =
include_dev_i2c_HEADERS += include/dev/i2c/eeprom.h
include_dev_i2c_HEADERS += include/dev/i2c/gpio-nxp-pca9535.h
include_dev_i2c_HEADERS += include/dev/i2c/i2c.h
include_dev_i2c_HEADERS += include/dev/i2c/switch-nxp-pca9548a.h

include_dev_spidir = $(includedir)/dev/spi
include_dev_spi_HEADERS =
include_dev_spi_HEADERS += include/dev/spi/spi.h

include_dev_serialdir = $(includedir)/dev/serial
include_dev_serial_HEADERS =
include_dev_serial_HEADERS += include/dev/serial/sc16is752.h

include_linuxdir = $(includedir)/linux
include_linux_HEADERS =
include_linux_HEADERS += include/linux/i2c.h
include_linux_HEADERS += include/linux/i2c-dev.h

include_linux_spidir = $(includedir)/linux/spi
include_linux_spi_HEADERS =
include_linux_spi_HEADERS += include/linux/spi/spidev.h

noinst_LIBRARIES = libdev.a

libdev_a_SOURCES =
libdev_a_SOURCES += i2c/eeprom.c
libdev_a_SOURCES += i2c/gpio-nxp-pca9535.c
libdev_a_SOURCES += i2c/i2c-bus.c
libdev_a_SOURCES += i2c/i2c-dev.c
libdev_a_SOURCES += i2c/switch-nxp-pca9548a.c
libdev_a_SOURCES += spi/spi-bus.c
libdev_a_SOURCES += serial/sc16is752.c
libdev_a_SOURCES += serial/sc16is752-spi.c
libdev_a_SOURCES += serial/sc16is752-regs.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
