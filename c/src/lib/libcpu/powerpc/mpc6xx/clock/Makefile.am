##
## $Id$
##

C_FILES = c_clock.c

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = c_clock.h

c_clock_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

all-local: $(PREINSTALL_FILES) $(c_clock_rel_OBJECTS)

EXTRA_DIST = c_clock.c c_clock.h

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/c_clock.h: c_clock.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/c_clock.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/c_clock.h

include $(top_srcdir)/../../../automake/local.am
