##
##  $Id$
##

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

if LIBCHIP
include_libchipdir = $(includedir)/libchip
include_libchip_HEADERS =
TMP_LIBS =
endif

EXTRA_DIST =
noinst_HEADERS =

# ide
libide_C_FILES = ide/ata.c ide/ide_controller.c
libide_OBJS = $(libide_C_FILES:ide/%.c=ide/$(ARCH)/%.$(OBJEXT))

noinst_HEADERS += ide/ata_internal.h

if LIBCHIP
include_libchip_HEADERS += ide/ata.h ide/ide_ctrl_cfg.h ide/ide_ctrl.h \
    ide/ide_ctrl_io.h

ide/$(ARCH)/$(dirstamp):
	@$(mkinstalldirs) ide/$(ARCH)
	@: > ide/$(ARCH)/$(dirstamp)
ide/$(ARCH)/%.$(OBJEXT): ide/%.c ide/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
ide/$(ARCH)/libide.a: $(libide_OBJS)
	$(make-library)
TMP_LIBS += ide/$(ARCH)/libide.a
endif

EXTRA_DIST += $(libide_C_FILES)

# network

libnetchip_C_FILES = network/cs8900.c network/dec21140.c network/i82586.c \
    network/sonic.c network/if_fxp.c network/elnk.c network/open_eth.c
libnetchip_OBJS = \
    $(libnetchip_C_FILES:network/%.c=network/$(ARCH)/%.$(OBJEXT))

libnetchip_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__

if LIBCHIP
if HAS_NETWORKING
include_libchip_HEADERS += network/cs8900.h network/i82586var.h \
    network/if_fxpvar.h network/sonic.h network/if_media.h network/mii.h \
    network/open_eth.h

network/$(ARCH)/$(dirstamp):
	@$(mkinstalldirs) network/$(ARCH)
	@: > network/$(ARCH)/$(dirstamp)
network/$(ARCH)/%.$(OBJEXT): network/%.c network/$(ARCH)/$(dirstamp)
	${COMPILE} $(libnetchip_CPPFLAGS) -o $@ -c $<
network/$(ARCH)/libnetchip$(LIB_VARIANT).a: $(libnetchip_OBJS)
	$(make-library)
TMP_LIBS += network/$(ARCH)/libnetchip$(LIB_VARIANT).a

project_lib_DATA = network/$(ARCH)/libnetchip$(LIB_VARIANT).a
endif
endif

libnetchip_DOC_FILES = network/README network/README.cs8900 \
    network/README.dec21140 network/README.i82586 network/README.open_eth \
    network/README.sonic network/cs8900.c.bsp

EXTRA_DIST += $(libnetchip_C_FILES) $(libnetchip_DOC_FILES)

# rtc
librtcio_C_FILES = rtc/rtcprobe.c rtc/icm7170.c rtc/icm7170_reg.c \
    rtc/icm7170_reg2.c rtc/icm7170_reg4.c rtc/icm7170_reg8.c rtc/m48t08.c \
    rtc/m48t08_reg.c rtc/m48t08_reg2.c rtc/m48t08_reg4.c rtc/m48t08_reg8.c

librtcio_OBJS = $(librtcio_C_FILES:rtc/%.c=rtc/$(ARCH)/%.$(OBJEXT))

if LIBCHIP
include_libchip_HEADERS += rtc/rtc.h rtc/icm7170.h rtc/m48t08.h

rtc/$(ARCH)/$(dirstamp):
	@$(mkinstalldirs) rtc/$(ARCH)
	@: > rtc/$(ARCH)/$(dirstamp)
rtc/$(ARCH)/%.$(OBJEXT): rtc/%.c rtc/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
rtc/$(ARCH)/librtcio.a: $(librtcio_OBJS)
	$(make-library)
TMP_LIBS += rtc/$(ARCH)/librtcio.a
endif

librtcio_DOC_FILES = rtc/README.ds1643 rtc/README.icm7170 rtc/README.m48t08 \
    rtc/README.m48t18 rtc/STATUS

EXTRA_DIST += $(librtcio_C_FILES) $(librtcio_DOC_FILES)

# serial
libserialio_C_FILES = serial/mc68681.c serial/mc68681_baud.c \
    serial/mc68681_reg.c serial/mc68681_reg2.c serial/mc68681_reg4.c \
    serial/mc68681_reg8.c serial/ns16550.c serial/z85c30.c \
    serial/z85c30_reg.c serial/serprobe.c serial/termios_baud2index.c \
    serial/termios_baud2num.c
libserialio_OBJS = \
    $(libserialio_C_FILES:serial/%.c=serial/$(ARCH)/%.$(OBJEXT))

noinst_HEADERS += serial/mc68681_p.h serial/ns16550_p.h serial/z85c30_p.h

if LIBCHIP
include_libchip_HEADERS += serial/mc68681.h serial/ns16550.h serial/z85c30.h \
    serial/serial.h serial/sersupp.h

serial/$(ARCH)/$(dirstamp):
	@$(mkinstalldirs) serial/$(ARCH)
	@: > serial/$(ARCH)/$(dirstamp)
serial/$(ARCH)/%.$(OBJEXT): serial/%.c serial/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
serial/$(ARCH)/libserialio.a: $(libserialio_OBJS)
	$(make-library)
TMP_LIBS += serial/$(ARCH)/libserialio.a
endif

libserialio_DOC_FILES = serial/README.mc68681 serial/README.ns16550 \
    serial/README.xr88681 serial/README.z85c30 serial/STATUS

all-local: $(PREINSTALL_FILES) $(TMP_LIBS) $(TMPINSTALL_FILES)

EXTRA_DIST += $(libserialio_C_FILES) $(libserialio_DOC_FILES)

CLEANDIRS = */$(ARCH)

PREINSTALL_FILES =

if LIBCHIP
$(PROJECT_INCLUDE)/libchip/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libchip
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

${PROJECT_RELEASE}/lib/libnetchip$(LIB_VARIANT).a: network/$(ARCH)/libnetchip$(LIB_VARIANT).a ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/libnetchip$(LIB_VARIANT).a
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/libnetchip$(LIB_VARIANT).a
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

include $(top_srcdir)/automake/local.am
