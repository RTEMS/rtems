##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.5

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

include_HEADERS = confdefs.h

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = rtems/config.h rtems/directives.h rtems/extension.h \
    rtems/fatal.h rtems/init.h rtems/io.h rtems/mptables.h rtems/sptables.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/%)

EXTRA_DIST = rtems/README

rtems/sptables.h: $(top_builddir)/config.status $(top_srcdir)/sapi/include/rtems/sptables.h.in
	cd $(top_builddir) && CONFIG_FILES=sapi/include/$@ CONFIG_HEADERS= CONFIG_LINKS= $(SHELL) ./config.status

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
