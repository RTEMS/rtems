##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = ftp.h inet.h nameser.h nameser_compat.h telnet.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa $(H_FILES:%=$(PROJECT_INCLUDE)/arpa/%)

$(PROJECT_INCLUDE)/arpa:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/arpa/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
