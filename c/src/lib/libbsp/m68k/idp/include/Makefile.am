##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_HEADERS = bsp.h coverhd.h leds.h bspopts.h

coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h
	cp $< $@
CLEANFILES += coverhd.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = README

include $(top_srcdir)/../../../../../../automake/local.am
