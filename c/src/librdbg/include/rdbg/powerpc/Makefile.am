#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

H_PIECES = rdbg_f.h reg.h

noinst_HEADERS = $(H_PIECES)

##TMPINSTALL_FILES = \
##$(PROJECT_INCLUDE)/rdbg/powerpc \
##$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rdbg/powerpc/%)

##$(PROJECT_INCLUDE)/rdbg/powerpc:
##	@$(mkinstalldirs) $@
##$(PROJECT_INCLUDE)/rdbg/powerpc/%.h: %.h
##	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/rdbg \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rdbg/%)

$(PROJECT_INCLUDE)/rdbg:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rdbg/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
