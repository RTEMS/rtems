##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.5

include_rtems_rtemsdir = $(includedir)/rtems/rtems

$(PROJECT_INCLUDE)/rtems/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%: %
	$(INSTALL_DATA) $< $@

if INLINE
include_rtems_rtems_HEADERS = rtems/rtems/asr.inl rtems/rtems/attr.inl \
    rtems/rtems/dpmem.inl rtems/rtems/event.inl rtems/rtems/eventset.inl \
    rtems/rtems/message.inl rtems/rtems/modes.inl rtems/rtems/options.inl \
    rtems/rtems/part.inl rtems/rtems/ratemon.inl rtems/rtems/region.inl \
    rtems/rtems/sem.inl rtems/rtems/status.inl rtems/rtems/support.inl \
    rtems/rtems/tasks.inl rtems/rtems/timer.inl

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems/rtems \
    $(include_rtems_rtems_HEADERS:%=$(PROJECT_INCLUDE)/%)
endif

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
