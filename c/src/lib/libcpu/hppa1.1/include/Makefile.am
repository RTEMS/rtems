##
## $Id$
##


include_hppa1_1dir = $(includedir)/hppa1.1

include_hppa1_1_HEADERS = runway.h semaphore.h

# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts

$(PROJECT_INCLUDE)/hppa1.1:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/hppa1.1/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/hppa1.1 \
    $(include_hppa1_1_HEADERS:%=$(PROJECT_INCLUDE)/hppa1.1/%)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = runway.h semaphore.h

include $(top_srcdir)/../../../automake/local.am
