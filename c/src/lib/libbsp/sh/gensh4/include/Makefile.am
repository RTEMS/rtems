##
## $Id$
##


include_HEADERS = bsp.h bspopts.h coverhd.h sdram.h

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
	$(include_HEADERS:%.h=$(PROJECT_INCLUDE)/%.h)

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
