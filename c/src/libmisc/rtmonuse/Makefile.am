## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = librtmonuse-tmp
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = rtmonuse.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

H_FILES = rtmonuse.h

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/lib.am

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(H_FILES:%=$(PROJECT_INCLUDE)/rtems/%)

all: $(ARCH) $(PREINSTALL_FILES) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = rtmonuse.c rtmonuse.h

include $(top_srcdir)/../../../automake/local.am
