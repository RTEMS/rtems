##
## $Id$
##


include_idedir = $(includedir)/libchip

include_ide_HEADERS = ide_ctrl_cfg.h ide_ctrl.h ide_ctrl_io.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libchip \
    $(include_ide_HEADERS:%=$(PROJECT_INCLUDE)/libchip/%)

$(PROJECT_INCLUDE)/libchip:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/libchip/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = 

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
