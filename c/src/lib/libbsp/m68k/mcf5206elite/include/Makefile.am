## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h coverhd.h i2c.h \
          i2cdrv.h ds1307.h nvram.h \
          tod.h bspopts.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/tod.h: $(top_srcdir)/../../shared/tod.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = $(H_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
