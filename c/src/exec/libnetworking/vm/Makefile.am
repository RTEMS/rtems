##
## $Id$
##




include_vmdir = $(includedir)/vm

include_vm_HEADERS = vm.h vm_extern.h vm_kern.h vm_param.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/vm \
    $(include_vm_HEADERS:%=$(PROJECT_INCLUDE)/vm/%)

$(PROJECT_INCLUDE)/vm:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/vm/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
