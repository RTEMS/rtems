##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4



include_arpadir = $(includedir)/arpa

include_arpa_HEADERS = ftp.h inet.h nameser.h nameser_compat.h telnet.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa \
    $(include_arpa_HEADERS:%=$(PROJECT_INCLUDE)/arpa/%)

$(PROJECT_INCLUDE)/arpa:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/arpa/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
