##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_RDBG
H_FILES = rdbg.h servrpc.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES = \
$(PROJECT_INCLUDE)/rdbg \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rdbg/%)

$(PROJECT_INCLUDE)/rdbg:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rdbg/%.h: %.h
	$(INSTALL_DATA) $< $@
endif

all-am: $(PREINSTALL_FILES)
debug-am: $(PREINSTALL_FILES)
profile-am: $(PREINSTALL_FILES)

if HAS_RDBG
SUBDIRS = $(RTEMS_CPU)
endif

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
