##
## $Id$
##

include_rdbgdir = $(includedir)/rdbg

include_rdbg_HEADERS = rdbg_f.h reg.h

all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rdbg/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rdbg
	@: > $(PROJECT_INCLUDE)/rdbg/$(dirstamp)

$(PROJECT_INCLUDE)/rdbg/rdbg_f.h: rdbg_f.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/rdbg_f.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/rdbg_f.h

$(PROJECT_INCLUDE)/rdbg/reg.h: reg.h $(PROJECT_INCLUDE)/rdbg/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rdbg/reg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rdbg/reg.h

include $(top_srcdir)/automake/local.am
