##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/exceptions.rel

C_FILES = raw_exception.c

S_FILES = asm_utils.S

H_FILES = raw_exception.h

exceptions_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o) \
    $(S_FILES:%.S=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(exceptions_rel_OBJECTS)
	$(make-rel)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu \
    $(PROJECT_INCLUDE)/libcpu/raw_exception.h

all-local: $(ARCH) $(PREINSTALL_FILES) $(exceptions_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = asm_utils.S raw_exception.c raw_exception.h

include $(top_srcdir)/../../../../../automake/local.am
