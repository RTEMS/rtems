## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bspio.h

C_FILES = printk.c

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h)

all: $(PREINSTALL_FILES)

EXTRA_DIST = bspio.h printk.c

include $(top_srcdir)/../../../../../automake/local.am
