##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES = c_clock.c

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = c_clock.h

c_clock_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o)

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

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libcpu \
    $(include_libcpu_HEADERS:%=$(PROJECT_INCLUDE)/libcpu/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(c_clock_rel_OBJECTS)

EXTRA_DIST = c_clock.c c_clock.h

include $(top_srcdir)/../../../../../automake/local.am
