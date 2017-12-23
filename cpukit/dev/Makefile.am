include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/multilib.am

noinst_LIBRARIES = libdev.a

libdev_a_SOURCES =
libdev_a_SOURCES += i2c/eeprom.c
libdev_a_SOURCES += i2c/fpga-i2c-slave.c
libdev_a_SOURCES += i2c/gpio-nxp-pca9535.c
libdev_a_SOURCES += i2c/i2c-bus.c
libdev_a_SOURCES += i2c/i2c-dev.c
libdev_a_SOURCES += i2c/sensor-lm75a.c
libdev_a_SOURCES += i2c/switch-nxp-pca9548a.c
libdev_a_SOURCES += i2c/xilinx-axi-i2c.c
libdev_a_SOURCES += i2c/ti-ads-16bit-adc.c
libdev_a_SOURCES += i2c/ti-lm25066a.c
libdev_a_SOURCES += i2c/ti-tmp112.c
libdev_a_SOURCES += spi/spi-bus.c
libdev_a_SOURCES += serial/sc16is752.c
libdev_a_SOURCES += serial/sc16is752-spi.c
libdev_a_SOURCES += serial/sc16is752-regs.h

include $(top_srcdir)/automake/local.am
