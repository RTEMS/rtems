##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

include_bspdir = $(includedir)/bsp

include_bsp_HEADERS = $(top_srcdir)/../../shared/vmeUniverse/vmeUniverse.h
shared_vmeUniverse_C_FILES = \
    $(top_srcdir)/../../shared/vmeUniverse/vmeUniverse.c
OBJS = \
    $(shared_vmeUniverse_C_FILES:$(top_srcdir)/../../shared/vmeUniverse/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../shared/vmeUniverse/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

include_bsp_HEADERS += $(top_srcdir)/../../powerpc/shared/vme/VMEConfig.h
include_bsp_HEADERS += $(top_srcdir)/../../powerpc/shared/vme/VME.h
powerpc_shared_vme_C_FILES = \
    $(top_srcdir)/../../powerpc/shared/vme/vmeconfig.c
OBJS += \
    $(powerpc_sharec_vme_C_FILES:$(top_srcdir)/../../powerpc/shared/vme/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../powerpc/shared/vme/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

noinst_DATA = $(OBJS)

all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/vmeUniverse.h: $(top_srcdir)/../../shared/vmeUniverse/vmeUniverse.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vmeUniverse.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vmeUniverse.h

$(PROJECT_INCLUDE)/bsp/VMEConfig.h: $(top_srcdir)/../../powerpc/shared/vme/VMEConfig.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/VMEConfig.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VMEConfig.h

$(PROJECT_INCLUDE)/bsp/VME.h: $(top_srcdir)/../../powerpc/shared/vme/VME.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/VME.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VME.h

include $(top_srcdir)/../../../../automake/local.am
