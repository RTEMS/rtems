##
## $Id$
##


include_idedir = $(includedir)/libchip

LIBNAME = libide
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = ide_controller.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

noinst_HEADERS = 

include_ide_HEADERS = ide_ctrl_cfg.h ide_ctrl.h ide_ctrl_io.h

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libchip \
    $(include_ide_HEADERS:%=$(PROJECT_INCLUDE)/libchip/%)

$(PROJECT_INCLUDE)/libchip:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/libchip/%.h: %.h
	$(INSTALL_DATA) $< $@

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am