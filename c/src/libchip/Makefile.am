include $(top_srcdir)/automake/compile.am

EXTRA_DIST =
noinst_LIBRARIES =
noinst_PROGRAMS =

# display
noinst_LIBRARIES += libdisplay.a
libdisplay_a_SOURCES = display/disp_hcms29xx.c display/font_hcms29xx.c
libdisplay_a_CPPFLAGS = $(AM_CPPFLAGS)

# flash
noinst_LIBRARIES += libflash.a
libflash_a_SOURCES = flash/am29lv160.c
libflash_a_CPPFLAGS = $(AM_CPPFLAGS)

# ide
noinst_LIBRARIES += libide.a
libide_a_SOURCES = ide/ata.c ide/ata_util.c ide/ide_controller.c
libide_a_CPPFLAGS = $(AM_CPPFLAGS)

# network
if HAS_NETWORKING
noinst_LIBRARIES += libnetchip.a
libnetchip_a_CPPFLAGS = $(AM_CPPFLAGS)
libnetchip_a_SOURCES = network/cs8900.c network/dec21140.c network/i82586.c \
    network/sonic.c network/if_fxp.c network/elnk.c network/open_eth.c \
    network/if_dc.c
if !HAS_SMP
libnetchip_a_SOURCES += network/greth.c
endif
libnetchip_a_SOURCES += network/smc91111.c network/smc91111config.h
endif

EXTRA_DIST += network/README network/README.cs8900 network/README.dec21140 \
    network/README.i82586 network/README.open_eth network/README.sonic \
    network/cs8900.c.bsp network/README.tulipclone

# rtc
noinst_LIBRARIES += librtcio.a
librtcio_a_CPPFLAGS = $(AM_CPPFLAGS)
librtcio_a_SOURCES = rtc/rtcprobe.c rtc/icm7170.c rtc/icm7170_reg.c \
    rtc/icm7170_reg2.c rtc/icm7170_reg4.c rtc/icm7170_reg8.c rtc/m48t08.c \
    rtc/m48t08_reg.c rtc/m48t08_reg2.c rtc/m48t08_reg4.c rtc/m48t08_reg8.c \
    rtc/mc146818a.c rtc/mc146818a_ioreg.c rtc/ds1375.c

EXTRA_DIST += rtc/README.ds1643 rtc/README.icm7170 rtc/README.m48t08 \
    rtc/README.m48t18 rtc/STATUS

# i2c
noinst_LIBRARIES += libi2cio.a

libi2cio_a_CPPFLAGS = $(AM_CPPFLAGS)
libi2cio_a_SOURCES = i2c/i2c-ds1621.h \
	i2c/i2c-2b-eeprom.h \
	i2c/spi-memdrv.h \
	i2c/spi-flash-m25p40.h \
	i2c/spi-fram-fm25l256.h \
	i2c/i2c-ds1621.c \
	i2c/i2c-2b-eeprom.c \
	i2c/i2c-sc620.c \
	i2c/spi-memdrv.c \
	i2c/spi-flash-m25p40.c \
	i2c/spi-fram-fm25l256.c \
	i2c/spi-sd-card.c

# serial
noinst_LIBRARIES += libserialio.a
libserialio_a_CPPFLAGS = $(AM_CPPFLAGS)
libserialio_a_SOURCES = serial/mc68681.c serial/mc68681_baud.c \
    serial/mc68681_reg.c serial/mc68681_reg2.c serial/mc68681_reg4.c \
    serial/mc68681_reg8.c serial/ns16550.c serial/z85c30.c \
    serial/z85c30_reg.c serial/serprobe.c serial/mc68681_p.h \
    serial/z85c30_p.h
libserialio_a_SOURCES += serial/ns16550-context.c

EXTRA_DIST += serial/README.mc68681 serial/README.ns16550 \
    serial/README.xr88681 serial/README.z85c30 serial/STATUS

## shmdr
if HAS_MP
project_lib_PROGRAMS = shmdr.rel
shmdr_rel_SOURCES = shmdr/addlq.c shmdr/cnvpkt.c shmdr/getlq.c shmdr/dump.c \
    shmdr/fatal.c shmdr/getpkt.c shmdr/init.c shmdr/initlq.c shmdr/intr.c \
    shmdr/mpisr.c shmdr/poll.c shmdr/receive.c shmdr/retpkt.c shmdr/send.c
shmdr_rel_CPPFLAGS = $(AM_CPPFLAGS)
shmdr_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

EXTRA_DIST += shmdr/README

## --

include $(top_srcdir)/automake/local.am
