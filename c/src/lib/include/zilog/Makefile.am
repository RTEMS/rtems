##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_zilogdir = ${includedir}/zilog

include_zilog_HEADERS = z8036.h z8530.h z8536.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/zilog \
    $(include_zilog_HEADERS:%=$(PROJECT_INCLUDE)/zilog/%)

$(PROJECT_INCLUDE)/zilog:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/zilog/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
