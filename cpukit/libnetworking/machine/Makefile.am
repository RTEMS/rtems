##
## $Id$
##


include_machinedir = $(includedir)/machine

include_machine_HEADERS = conf.h cpu.h cpufunc.h endian.h in_cksum.h vmparam.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/machine \
    $(include_machine_HEADERS:%=$(PROJECT_INCLUDE)/machine/%)

$(PROJECT_INCLUDE)/machine:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/machine/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
