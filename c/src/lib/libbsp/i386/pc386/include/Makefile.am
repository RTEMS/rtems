## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

# This driver needs to be reworked for the BSD stack.
# We only install wd80x3.h if HAS_NETWORKING was defined
if HAS_NETWORKING
WD80X3 = wd80x3.h
endif

H_FILES = bsp.h crt.h $(WD80X3)

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h) \
    $(PROJECT_INCLUDE)/coverhd.h

all: $(PREINSTALL_FILES)

EXTRA_DIST = bsp.h crt.h wd80x3.h

include $(top_srcdir)/../../../../../../automake/local.am
