##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if MACROS
I_PIECES = asr.inl attr.inl dpmem.inl event.inl eventset.inl message.inl \
    modes.inl options.inl part.inl ratemon.inl \
    region.inl sem.inl status.inl support.inl tasks.inl timer.inl

noinst_HEADERS = $(I_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems/rtems \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rtems/rtems/%)

$(PROJECT_INCLUDE)/rtems/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/rtems/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
