##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.5

include_HEADERS = chain.h console.h clockdrv.h iosupp.h ringbuf.h rtc.h \
    spurious.h timerdrv.h vmeintr.h

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

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

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

## motorola

include_motoroladir = $(includedir)/motorola

include_motorola_HEADERS = motorola/mc68230.h motorola/mc68681.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/motorola \
    $(include_motorola_HEADERS:%=$(PROJECT_INCLUDE)/%)

## rtems
include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = \
    rtems/assoc.h rtems/error.h rtems/libcsupport.h rtems/libio.h rtems/libio_.h \
    rtems/termiostypes.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/%)

## sys

include_sysdir = $(includedir)/sys

if !HAS_NETWORKING
NETWORKING_H_FILES = sys/ioctl.h
endif

if NEWLIB
NEWLIB_H_FILES = sys/termios.h sys/cdefs.h
endif

include_sys_HEADERS = \
sys/ioccom.h \
sys/ttycom.h \
sys/utime.h \
$(NEWLIB_H_FILES) $(NETWORKING_H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys \
    $(include_sys_HEADERS:%=$(PROJECT_INCLUDE)/%)

## zilog

include_zilogdir = $(includedir)/zilog

include_zilog_HEADERS = zilog/z8036.h zilog/z8530.h zilog/z8536.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/zilog \
    $(include_zilog_HEADERS:%=$(PROJECT_INCLUDE)/%)

##  General stuff
all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
