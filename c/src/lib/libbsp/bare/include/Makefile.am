## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
H_FILES = bsp.h coverhd.h

noinst_HEADERS = $(H_FILES)

TMPINSTALL_FILES += \
$(PROJECT_INCLUDE) \
$(H_FILES:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/local.am
