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

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(exceptions_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(PREINSTALL_FILES) $(exceptions_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = asm_utils.S raw_exception.c raw_exception.h

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/raw_exception.h: raw_exception.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/raw_exception.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/raw_exception.h

include $(top_srcdir)/../../../automake/local.am
