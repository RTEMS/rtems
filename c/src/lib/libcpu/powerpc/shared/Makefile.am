## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

## C source names
C_FILES = cpuIdent.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = cpu.h spr.h io.h mmu.h page.h byteorder.h pgtable.h
noinst_HEADERS = $(H_FILES)

## Assembly source names
S_FILES =
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

LIB = ${ARCH}/libcpuspec.a

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): ${OBJS}
	$(make-library)

PREINSTALL_FILES += \
$(PROJECT_INCLUDE)/libcpu \
$(H_FILES:%=$(PROJECT_INCLUDE)/libcpu/%)

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: ${ARCH} $(PREINSTALL_FILES) $(LIB)

EXTRA_DIST = $(C_FILES) $(S_FILES)

include $(top_srcdir)/../../../../../automake/local.am
