## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_NETWORKING
NETWORK_H_FILES = wd80x3.h
endif

H_FILES = bsp.h coverhd.h wd80x3.h $(NETWORK_H_FILES)

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h)

all: $(PREINSTALL_FILES)

EXTRA_DIST = bsp.h coverhd.h wd80x3.h

include $(top_srcdir)/../../../../../../automake/local.am
