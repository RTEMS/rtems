##
## $Id$
##

C_FILES = tbr.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = tbr.h

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libcpu \
    $(include_libcpu_HEADERS:%=$(PROJECT_INCLUDE)/libcpu/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

EXTRA_DIST = tbr.c

include $(top_srcdir)/../../../../../automake/local.am
