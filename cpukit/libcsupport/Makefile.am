##
##  $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/multilib.am

SUBDIRS = . src

include_HEADERS = include/chain.h include/console.h include/clockdrv.h \
    include/iosupp.h include/ringbuf.h include/rtc.h \
    include/spurious.h include/timerdrv.h include/vmeintr.h

LIBC_HFILES = include/stdint.h include/inttypes.h
include_HEADERS += $(LIBC_HFILES)

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/motorola:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/sys:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/zilog:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@

## motorola

include_motoroladir = $(includedir)/motorola

include_motorola_HEADERS = include/motorola/mc68230.h include/motorola/mc68681.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/motorola \
    $(include_motorola_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

## rtems
include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = \
    include/rtems/assoc.h include/rtems/error.h include/rtems/libcsupport.h \
    include/rtems/libio.h include/rtems/libio_.h \
    include/rtems/termiostypes.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

## sys

include_sysdir = $(includedir)/sys


if NEWLIB
NEWLIB_H_FILES = include/sys/termios.h include/sys/cdefs.h include/sys/utsname.h
endif

include_sys_HEADERS = \
include/sys/filio.h \
include/sys/ioccom.h \
include/sys/ioctl.h \
include/sys/sockio.h \
include/sys/termios.h \
include/sys/ttycom.h \
include/sys/utime.h \
$(NEWLIB_H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys \
    $(include_sys_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

## zilog

include_zilogdir = $(includedir)/zilog

include_zilog_HEADERS = include/zilog/z8036.h include/zilog/z8530.h \
    include/zilog/z8536.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/zilog \
    $(include_zilog_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

##  General stuff
all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
