##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_sysdir = $(includedir)/sys

if HAS_NETWORKING
else
NETWORKING_H_FILES += ioctl.h
endif

if NEWLIB
NEWLIB_H_FILES += termios.h cdefs.h
endif

include_sys_HEADERS = ioccom.h ttycom.h utime.h \
  $(NEWLIB_H_FILES) $(NETWORKING_H_FILES)

noinst_HEADERS = utime.h termios.h ioctl.h ioccom.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys \
    $(include_sys_HEADERS:%=$(PROJECT_INCLUDE)/sys/%)

$(PROJECT_INCLUDE)/sys:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/sys/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
