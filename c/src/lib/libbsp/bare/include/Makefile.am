##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: $(srcdir)/../../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h) \
    $(PROJECT_INCLUDE)/coverhd.h

all: $(TMPINSTALL_FILES)

EXTRA_DIST = bsp.h

include $(top_srcdir)/../../../../../automake/local.am
