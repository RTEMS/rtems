##
## $Id$
##


include_rdbgdir = $(includedir)/rdbg

include_rdbg_HEADERS = rdbg_f.h reg.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rdbg \
    $(include_rdbg_HEADERS:%=$(PROJECT_INCLUDE)/rdbg/%)

$(PROJECT_INCLUDE)/rdbg:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rdbg/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
