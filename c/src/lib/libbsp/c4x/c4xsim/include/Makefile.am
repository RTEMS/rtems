## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h simio.h 

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bspIo.h: $(top_srcdir)/../../shared/include/bspIo.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h) \
    $(PROJECT_INCLUDE)/bspIo.h $(PROJECT_INCLUDE)/coverhd.h 

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = bsp.h

include $(top_srcdir)/../../../../../../automake/local.am

H_FILES = $(srcdir)/../../../shared/include/coverhd.h \
    $(srcdir)/bsp.h $(srcdir)/simio.h $(srcdir)/../../../shared/include/bspIo.h

