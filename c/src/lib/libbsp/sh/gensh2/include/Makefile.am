## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h coverhd.h

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h)

all: $(PREINSTALL_FILES)

EXTRA_DIST = bsp.h coverhd.h

include $(top_srcdir)/../../../../../../automake/local.am
