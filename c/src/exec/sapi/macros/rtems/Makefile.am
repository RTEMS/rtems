#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

if MACROS
I_PIECES = extension.inl

noinst_HEADERS = $(I_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
