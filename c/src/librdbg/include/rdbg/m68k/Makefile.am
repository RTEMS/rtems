##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = rdbg_f.h reg.h

noinst_HEADERS = $(H_FILES)

##PREINSTALL_FILES = \
##$(PROJECT_INCLUDE)/rdbg/m68k \
##$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rdbg/m68k/%)

##$(PROJECT_INCLUDE)/rdbg/m68k:
##	@$(mkinstalldirs) $@
##$(PROJECT_INCLUDE)/rdbg/m68k/%.h: %.h
##	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg \
    $(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rdbg/%)

$(PROJECT_INCLUDE)/rdbg:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rdbg/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
