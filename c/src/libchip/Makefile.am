include $(top_srcdir)/automake/compile.am

include_libchipdir = $(includedir)/libchip
include_libchip_HEADERS =

EXTRA_DIST =
noinst_LIBRARIES =
noinst_PROGRAMS =

# display
include_libchip_HEADERS += display/disp_hcms29xx.h

noinst_LIBRARIES += libdisplay.a
libdisplay_a_SOURCES = display/disp_hcms29xx.c display/font_hcms29xx.c
libdisplay_a_CPPFLAGS = $(AM_CPPFLAGS)

# flash
include_libchip_HEADERS += flash/am29lv160.h

noinst_LIBRARIES += libflash.a
libflash_a_SOURCES = flash/am29lv160.c
libflash_a_CPPFLAGS = $(AM_CPPFLAGS)

# ide
include_libchip_HEADERS += ide/ata.h ide/ide_ctrl_cfg.h ide/ide_ctrl.h \
    ide/ide_ctrl_io.h ide/ata_internal.h

noinst_LIBRARIES += libide.a
libide_a_SOURCES = ide/ata.c ide/ata_util.c ide/ide_controller.c
libide_a_CPPFLAGS = $(AM_CPPFLAGS)

# network
if HAS_NETWORKING
noinst_LIBRARIES += libnetchip.a
libnetchip_a_CPPFLAGS = $(AM_CPPFLAGS)
libnetchip_a_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
include_libchip_HEADERS += network/cs8900.h network/i82586var.h \
    network/if_fxpvar.h network/sonic.h
include_libchip_HEADERS += network/open_eth.h network/if_dcreg.h
include_libchip_HEADERS += network/greth.h
libnetchip_a_SOURCES = network/cs8900.c network/dec21140.c network/i82586.c \
    network/sonic.c network/if_fxp.c network/elnk.c network/open_eth.c \
    network/if_dc.c
libnetchip_a_SOURCES += network/greth.c
include_libchip_HEADERS += network/smc91111.h network/smc91111exp.h
libnetchip_a_SOURCES += network/smc91111.c network/smc91111config.h
endif

EXTRA_DIST += network/README network/README.cs8900 network/README.dec21140 \
    network/README.i82586 network/README.open_eth network/README.sonic \
    network/cs8900.c.bsp network/README.tulipclone

# rtc
include_libchip_HEADERS += rtc/rtc.h rtc/icm7170.h rtc/m48t08.h \
    rtc/mc146818a.h rtc/ds1375-rtc.h

noinst_LIBRARIES += librtcio.a
librtcio_a_CPPFLAGS = $(AM_CPPFLAGS)
librtcio_a_SOURCES = rtc/rtcprobe.c rtc/icm7170.c rtc/icm7170_reg.c \
    rtc/icm7170_reg2.c rtc/icm7170_reg4.c rtc/icm7170_reg8.c rtc/m48t08.c \
    rtc/m48t08_reg.c rtc/m48t08_reg2.c rtc/m48t08_reg4.c rtc/m48t08_reg8.c \
    rtc/mc146818a.c rtc/mc146818a_ioreg.c rtc/ds1375.c

EXTRA_DIST += rtc/README.ds1643 rtc/README.icm7170 rtc/README.m48t08 \
    rtc/README.m48t18 rtc/STATUS

# i2c
include_libchip_HEADERS += i2c/i2c-ds1621.h \
	i2c/i2c-2b-eeprom.h \
	i2c/spi-memdrv.h \
	i2c/spi-flash-m25p40.h \
	i2c/spi-fram-fm25l256.h \
	i2c/spi-sd-card.h

noinst_LIBRARIES += libi2cio.a

libi2cio_a_CPPFLAGS = $(AM_CPPFLAGS)
libi2cio_a_SOURCES = i2c/i2c-ds1621.h \
	i2c/i2c-2b-eeprom.h \
	i2c/spi-memdrv.h \
	i2c/spi-flash-m25p40.h \
	i2c/spi-fram-fm25l256.h \
	i2c/i2c-ds1621.c \
	i2c/i2c-2b-eeprom.c \
	i2c/spi-memdrv.c \
	i2c/spi-flash-m25p40.c \
	i2c/spi-fram-fm25l256.c \
	i2c/spi-sd-card.c

# serial
include_libchip_HEADERS += serial/mc68681.h serial/ns16550.h serial/z85c30.h \
    serial/serial.h serial/sersupp.h

noinst_LIBRARIES += libserialio.a
libserialio_a_CPPFLAGS = $(AM_CPPFLAGS)
libserialio_a_SOURCES = serial/mc68681.c serial/mc68681_baud.c \
    serial/mc68681_reg.c serial/mc68681_reg2.c serial/mc68681_reg4.c \
    serial/mc68681_reg8.c serial/ns16550.c serial/z85c30.c \
    serial/z85c30_reg.c serial/serprobe.c serial/mc68681_p.h \
    serial/ns16550_p.h serial/z85c30_p.h

EXTRA_DIST += serial/README.mc68681 serial/README.ns16550 \
    serial/README.xr88681 serial/README.z85c30 serial/STATUS

## shmdr
if HAS_MP
include_HEADERS = shmdr/shm_driver.h shmdr/mpci.h

project_lib_PROGRAMS = shmdr.rel
shmdr_rel_SOURCES = shmdr/addlq.c shmdr/cnvpkt.c shmdr/getlq.c shmdr/dump.c \
    shmdr/fatal.c shmdr/getpkt.c shmdr/init.c shmdr/initlq.c shmdr/intr.c \
    shmdr/mpisr.c shmdr/poll.c shmdr/receive.c shmdr/retpkt.c shmdr/send.c
shmdr_rel_CPPFLAGS = $(AM_CPPFLAGS)
shmdr_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

EXTRA_DIST += shmdr/README

## --

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
