##
## $Id$
##

if HAS_RDBG
include_rdbgdir = $(includedir)/rdbg

include_rdbg_HEADERS = rdbg.h servrpc.h
endif

all-local: $(PREINSTALL_FILES)

if HAS_RDBG
SUBDIRS = $(RTEMS_CPU)
DIST_SUBDIRS = $(RTEMS_CPU)
endif

PREINSTALL_FILES =

if HAS_RDBG
$(PROJECT_INCLUDE)/rdbg/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rdbg
	@: > $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/$(dirstamp)

$(PROJECT_INCLUDE)/rdbg/rdbg.h: rdbg.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/rdbg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/rdbg.h

$(PROJECT_INCLUDE)/rdbg/servrpc.h: servrpc.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/servrpc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/servrpc.h
endif

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
