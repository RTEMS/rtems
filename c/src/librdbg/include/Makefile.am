##
## $Id$
##

## FIXME: This all is one blatant ugly HACK
## May-be this should be put into libcpu?

if HAS_RDBG
include_rdbgdir = $(includedir)/rdbg

include_rdbg_HEADERS = rdbg/rdbg.h rdbg/servrpc.h
nodist_include_rdbg_HEADERS = rdbg/@RTEMS_CPU@/rdbg_f.h \
    rdbg/@RTEMS_CPU@/reg.h

EXTRA_DIST = rdbg/i386/rdbg_f.h rdbg/i386/reg.h
EXTRA_DIST += rdbg/m68k/rdbg_f.h rdbg/m68k/reg.h
EXTRA_DIST += rdbg/powerpc/rdbg_f.h rdbg/powerpc/reg.h
endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if HAS_RDBG
$(PROJECT_INCLUDE)/rdbg/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rdbg
	@: > $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rdbg/$(dirstamp)

$(PROJECT_INCLUDE)/rdbg/rdbg.h: rdbg/rdbg.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/rdbg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/rdbg.h

$(PROJECT_INCLUDE)/rdbg/servrpc.h: rdbg/servrpc.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/servrpc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/servrpc.h

$(PROJECT_INCLUDE)/rdbg/rdbg_f.h: rdbg/@RTEMS_CPU@/rdbg_f.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/rdbg_f.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/rdbg_f.h

$(PROJECT_INCLUDE)/rdbg/reg.h: rdbg/@RTEMS_CPU@/reg.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/reg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/reg.h
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
