##
## $Id$
##


include_HEADERS = bsp.h coverhd.h mvme16x_hw.h page_table.h fatal.h bspopts.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

mvme16x_hw.h: $(srcdir)/../../shared/mvme/mvme16x_hw.h
	cp $< $@
CLEANFILES = mvme16x_hw.h

TMPINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
