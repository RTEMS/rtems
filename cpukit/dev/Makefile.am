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

include_linuxdir = $(includedir)/linux
include_linux_HEADERS =
include_linux_HEADERS += include/linux/i2c.h
include_linux_HEADERS += include/linux/i2c-dev.h

noinst_LIBRARIES = libdev.a

libdev_a_SOURCES =
libdev_a_SOURCES += i2c/eeprom.c
libdev_a_SOURCES += i2c/gpio-nxp-pca9535.c
libdev_a_SOURCES += i2c/i2c-bus.c
libdev_a_SOURCES += i2c/i2c-dev.c
libdev_a_SOURCES += i2c/switch-nxp-pca9548a.c

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
