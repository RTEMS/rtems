##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = runway.h semaphore.h

# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts

$(PROJECT_INCLUDE)/hppa1.1:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/hppa1.1/%.h: %.h
	$(INSTALL_DATA) $@ $<

TMPINSTALL_FILES += $(PROJECT_INCLUDE)/hppa1.1 \
    $(H_FILES:%.h=$(PROJECT_INCLUDE)/hppa1.1/%.h)

all: $(TMPINSTALL_FILES)

EXTRA_DIST = runway.h semaphore.h

include $(top_srcdir)/../../../../../automake/local.am
