##
## $Id$
##

if HAS_RDBG
include_rdbgdir = $(includedir)/rdbg

include_rdbg_HEADERS = rdbg.h servrpc.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rdbg \
    $(include_rdbg_HEADERS:%=$(PROJECT_INCLUDE)/rdbg/%)

$(PROJECT_INCLUDE)/rdbg:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rdbg/%.h: %.h
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

if HAS_RDBG
SUBDIRS = $(RTEMS_CPU)
DIST_SUBDIRS = $(RTEMS_CPU)
endif

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
