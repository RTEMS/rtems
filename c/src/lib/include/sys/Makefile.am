##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_NETWORKING
else
NETWORKING_H_PIECES += ioctl.h
endif

if UNIX
else
NEWLIB_H_PIECES += termios.h
endif

H_PIECES += utime.h $(NEWLIB_H_PIECES) $(NETWORKING_H_PIECES)

noinst_HEADERS = $(H_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/sys \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/sys/%)

$(PROJECT_INCLUDE)/sys:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/sys/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
