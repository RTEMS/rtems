## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

## C sources
C_FILES = c_clock.c

H_FILES = c_clock.h
noinst_HEADERS = $(H_FILES)

c_clock_rel_OBJECTS = $(C_FILES:%.c=${ARCH}/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu \
$(PROJECT_INCLUDE)/libcpu/c_clock.h

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS = $(CFLAGS_OS_V)

all-local: ${ARCH} $(PREINSTALL_FILES) $(c_clock_rel_OBJECTS)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../../../automake/local.am
