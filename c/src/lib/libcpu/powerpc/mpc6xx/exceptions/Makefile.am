## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/exceptions.rel

## C sources
C_FILES = raw_exception.c

## Assembly sources
S_FILES = asm_utils.S

H_FILES = raw_exception.h
noinst_HEADERS = $(H_FILES)

exceptions_rel_OBJECTS = $(C_FILES:%.c=${ARCH}/%.o) $(S_FILES:%.S=${ARCH}/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu \
$(PROJECT_INCLUDE)/libcpu/raw_exception.h

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS = $(CFLAGS_OS_V)

$(PGM): $(exceptions_rel_OBJECTS)
	$(make-rel)

all-local: ${ARCH} $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES) $(S_FILES)

include $(top_srcdir)/../../../../../automake/local.am
