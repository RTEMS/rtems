#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

if MACROS
I_PIECES =

noinst_HEADERS = $(I_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems/posix \
$(I_PIECES:%=$(PROJECT_INCLUDE)/rtems/posix/%)

$(PROJECT_INCLUDE)/rtems/posix:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/posix/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
