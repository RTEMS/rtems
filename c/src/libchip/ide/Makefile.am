##
## $Id$
##

LIBNAME = libide
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = ata.c ide_controller.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

noinst_HEADERS = ata_internal.h

OBJS = $(C_O_FILES)

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

if LIBCHIP
include_idedir = $(includedir)/libchip
include_ide_HEADERS = ata.h ide_ctrl_cfg.h ide_ctrl.h ide_ctrl_io.h

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB)
endif

EXTRA_DIST = $(C_FILES)

PREINSTALL_FILES =

if LIBCHIP
$(PROJECT_INCLUDE)/libchip/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libchip
	@: > $(PROJECT_INCLUDE)/libchip/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/$(dirstamp)

$(PROJECT_INCLUDE)/libchip/ata.h: ata.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/ata.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/ata.h

$(PROJECT_INCLUDE)/libchip/ide_ctrl_cfg.h: ide_ctrl_cfg.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/ide_ctrl_cfg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/ide_ctrl_cfg.h

$(PROJECT_INCLUDE)/libchip/ide_ctrl.h: ide_ctrl.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/ide_ctrl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/ide_ctrl.h

$(PROJECT_INCLUDE)/libchip/ide_ctrl_io.h: ide_ctrl_io.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/ide_ctrl_io.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/ide_ctrl_io.h
endif

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
