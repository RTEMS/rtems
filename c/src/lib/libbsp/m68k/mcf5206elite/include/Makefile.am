## 
## $Id$
## 


include_HEADERS = bsp.h coverhd.h i2c.h \
          i2cdrv.h ds1307.h nvram.h \
          tod.h bspopts.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

tod.h: $(top_srcdir)/../../shared/tod.h
	cp $< $@
CLEANFILES = tod.h

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
   $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
