##
##  $Id$
##

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

noinst_DATA =

if LIBCHIP
include_libchipdir = $(includedir)/libchip
include_libchip_HEADERS =
endif

EXTRA_DIST =
EXTRA_LIBRARIES =
EXTRA_PROGRAMS =
CLEANFILES =

# ide
if LIBCHIP
include_libchip_HEADERS += ide/ata.h ide/ide_ctrl_cfg.h ide/ide_ctrl.h \
    ide/ide_ctrl_io.h

EXTRA_LIBRARIES += libide.a
CLEANFILES += libide.a
libide_a_SOURCES = ide/ata.c ide/ide_controller.c ide/ata_internal.h
libide_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libide_g.a
CLEANFILES += libide_g.a
libide_g_a_SOURCES = $(libide_a_SOURCES)
libide_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

noinst_DATA += libide$(LIB_VARIANT).a
endif

# network
if LIBCHIP
if HAS_NETWORKING
include_libchip_HEADERS += network/cs8900.h network/i82586var.h \
    network/if_fxpvar.h network/sonic.h network/if_media.h network/mii.h \
    network/open_eth.h

libnetchip_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
EXTRA_LIBRARIES += libnetchip.a
CLEANFILES += libnetchip.a
libnetchip_a_SOURCES = network/cs8900.c network/dec21140.c network/i82586.c \
    network/sonic.c network/if_fxp.c network/elnk.c network/open_eth.c
libnetchip_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) \
    $(libnetchip_CPPFLAGS)

EXTRA_LIBRARIES += libnetchip_g.a
CLEANFILES += libnetchip_g.a
libnetchip_g_a_SOURCES = $(libnetchip_a_SOURCES)
libnetchip_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) \
    $(libnetchip_CPPFLAGS)

# FIXME: Why is this lib installed?
project_lib_DATA = libnetchip$(LIB_VARIANT).a
endif
endif

EXTRA_DIST += network/README network/README.cs8900 network/README.dec21140 \
    network/README.i82586 network/README.open_eth network/README.sonic \
    network/cs8900.c.bsp

# rtc
if LIBCHIP
include_libchip_HEADERS += rtc/rtc.h rtc/icm7170.h rtc/m48t08.h

EXTRA_LIBRARIES += librtcio.a
CLEANFILES += librtcio.a
librtcio_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
librtcio_a_SOURCES = rtc/rtcprobe.c rtc/icm7170.c rtc/icm7170_reg.c \
    rtc/icm7170_reg2.c rtc/icm7170_reg4.c rtc/icm7170_reg8.c rtc/m48t08.c \
    rtc/m48t08_reg.c rtc/m48t08_reg2.c rtc/m48t08_reg4.c rtc/m48t08_reg8.c

EXTRA_LIBRARIES += librtcio_g.a
CLEANFILES += librtcio_g.a
librtcio_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
librtcio_g_a_SOURCES = $(librtcio_a_SOURCES)

noinst_DATA += librtcio$(LIB_VARIANT).a
endif

EXTRA_DIST += rtc/README.ds1643 rtc/README.icm7170 rtc/README.m48t08 \
    rtc/README.m48t18 rtc/STATUS

# serial
if LIBCHIP
include_libchip_HEADERS += serial/mc68681.h serial/ns16550.h serial/z85c30.h \
    serial/serial.h serial/sersupp.h

EXTRA_LIBRARIES += libserialio.a
CLEANFILES += libserialio.a
libserialio_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
libserialio_a_SOURCES = serial/mc68681.c serial/mc68681_baud.c \
    serial/mc68681_reg.c serial/mc68681_reg2.c serial/mc68681_reg4.c \
    serial/mc68681_reg8.c serial/ns16550.c serial/z85c30.c \
    serial/z85c30_reg.c serial/serprobe.c serial/termios_baud2index.c \
    serial/termios_baud2num.c serial/mc68681_p.h serial/ns16550_p.h \
    serial/z85c30_p.h

EXTRA_LIBRARIES += libserialio_g.a
CLEANFILES += libserialio_g.a
libserialio_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
libserialio_g_a_SOURCES = $(libserialio_a_SOURCES)

noinst_DATA += libserialio$(LIB_VARIANT).a
endif

EXTRA_DIST += serial/README.mc68681 serial/README.ns16550 \
    serial/README.xr88681 serial/README.z85c30 serial/STATUS

## shmdr
if HAS_MP
include_HEADERS = shmdr/shm_driver.h shmdr/mpci.h

EXTRA_PROGRAMS += shmdr.rel
CLEANFILES += shmdr.rel
shmdr_rel_SOURCES = shmdr/addlq.c shmdr/cnvpkt.c shmdr/getlq.c shmdr/dump.c \
    shmdr/fatal.c shmdr/getpkt.c shmdr/init.c shmdr/initlq.c shmdr/intr.c \
    shmdr/mpisr.c shmdr/poll.c shmdr/receive.c shmdr/retpkt.c shmdr/send.c \
    shmdr/setckvec.c
shmdr_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
shmdr_rel_LDFLAGS = $(RTEMS_REL_LDFLAGS)

EXTRA_PROGRAMS += shmdr_g.rel
CLEANFILES += shmdr_g.rel
shmdr_g_rel_SOURCES = $(no_shmdr_rel_SOURCES)
shmdr_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
shmdr_g_rel_LDFLAGS = $(RTEMS_REL_LDFLAGS)

project_lib_DATA = shmdr$(LIB_VARIANT).rel
endif

EXTRA_DIST += shmdr/README

## --

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

PREINSTALL_FILES =
TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

if LIBCHIP
$(PROJECT_INCLUDE)/libchip/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libchip
	@: > $(PROJECT_INCLUDE)/libchip/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/$(dirstamp)
endif

if LIBCHIP
$(PROJECT_INCLUDE)/libchip/ata.h: ide/ata.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/ata.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/ata.h

$(PROJECT_INCLUDE)/libchip/ide_ctrl_cfg.h: ide/ide_ctrl_cfg.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/ide_ctrl_cfg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/ide_ctrl_cfg.h

$(PROJECT_INCLUDE)/libchip/ide_ctrl.h: ide/ide_ctrl.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/ide_ctrl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/ide_ctrl.h

$(PROJECT_INCLUDE)/libchip/ide_ctrl_io.h: ide/ide_ctrl_io.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/ide_ctrl_io.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/ide_ctrl_io.h
endif

if LIBCHIP
if HAS_NETWORKING
$(PROJECT_INCLUDE)/libchip/cs8900.h: network/cs8900.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/cs8900.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/cs8900.h

$(PROJECT_INCLUDE)/libchip/i82586var.h: network/i82586var.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/i82586var.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/i82586var.h

$(PROJECT_INCLUDE)/libchip/if_fxpvar.h: network/if_fxpvar.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/if_fxpvar.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/if_fxpvar.h

$(PROJECT_INCLUDE)/libchip/sonic.h: network/sonic.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/sonic.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/sonic.h

$(PROJECT_INCLUDE)/libchip/if_media.h: network/if_media.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/if_media.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/if_media.h

$(PROJECT_INCLUDE)/libchip/mii.h: network/mii.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/mii.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/mii.h

$(PROJECT_INCLUDE)/libchip/open_eth.h: network/open_eth.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/open_eth.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/open_eth.h

$(PROJECT_LIB)/libnetchip$(LIB_VARIANT).a: libnetchip$(LIB_VARIANT).a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/libnetchip$(LIB_VARIANT).a
TMPINSTALL_FILES += $(PROJECT_LIB)/libnetchip$(LIB_VARIANT).a
endif
endif

if LIBCHIP
$(PROJECT_INCLUDE)/libchip/rtc.h: rtc/rtc.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/rtc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/rtc.h

$(PROJECT_INCLUDE)/libchip/icm7170.h: rtc/icm7170.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/icm7170.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/icm7170.h

$(PROJECT_INCLUDE)/libchip/m48t08.h: rtc/m48t08.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/m48t08.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/m48t08.h
endif

if LIBCHIP
$(PROJECT_INCLUDE)/libchip/mc68681.h: serial/mc68681.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/mc68681.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/mc68681.h

$(PROJECT_INCLUDE)/libchip/ns16550.h: serial/ns16550.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/ns16550.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/ns16550.h

$(PROJECT_INCLUDE)/libchip/z85c30.h: serial/z85c30.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/z85c30.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/z85c30.h

$(PROJECT_INCLUDE)/libchip/serial.h: serial/serial.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/serial.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/serial.h

$(PROJECT_INCLUDE)/libchip/sersupp.h: serial/sersupp.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/sersupp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/sersupp.h
endif

if HAS_MP
$(PROJECT_INCLUDE)/shm_driver.h: shmdr/shm_driver.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/shm_driver.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/shm_driver.h

$(PROJECT_INCLUDE)/mpci.h: shmdr/mpci.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpci.h

$(PROJECT_LIB)/shmdr$(LIB_VARIANT).rel: shmdr$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/shmdr$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/shmdr$(LIB_VARIANT).rel
endif

CLEANFILES += $(PREINSTALL_FILES)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
