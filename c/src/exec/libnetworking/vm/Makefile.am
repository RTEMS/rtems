##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = vm.h vm_extern.h vm_kern.h vm_param.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/vm \
    $(H_FILES:%=$(PROJECT_INCLUDE)/vm/%)

$(PROJECT_INCLUDE)/vm:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/vm/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
