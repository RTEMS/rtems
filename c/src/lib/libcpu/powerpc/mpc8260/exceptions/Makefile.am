##
## $Id$
##


PGM = $(ARCH)/exceptions.rel

C_FILES = raw_exception.c

S_FILES = asm_utils.S

include_libcpudir = $(includedir)/libcpu
include_libcpu_HEADERS = raw_exception.h

exceptions_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT)) \
    $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(exceptions_rel_OBJECTS)
	$(make-rel)

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libcpu \
    $(include_libcpu_HEADERS:%=$(PROJECT_INCLUDE)/libcpu/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(exceptions_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = asm_utils.S raw_exception.c raw_exception.h

include $(top_srcdir)/../../../automake/local.am
