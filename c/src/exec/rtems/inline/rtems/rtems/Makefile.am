##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtems_rtemsdir = $(includedir)/rtems/rtems

I_FILES = asr.inl attr.inl dpmem.inl event.inl eventset.inl message.inl \
    modes.inl options.inl part.inl ratemon.inl region.inl sem.inl status.inl \
    support.inl tasks.inl timer.inl

if INLINE
include_rtems_rtems_HEADERS = $(I_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/rtems \
    $(include_rtems_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/rtems/%)

$(PROJECT_INCLUDE)/rtems/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/rtems/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = $(I_FILES)

include $(top_srcdir)/../../../automake/local.am
