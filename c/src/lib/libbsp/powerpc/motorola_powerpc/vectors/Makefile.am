##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../console:@srcdir@/../../shared/vectors

C_FILES = vectors_init.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

H_FILES = ../../shared/vectors/vectors.h

S_FILES = vectors.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

OBJS = $(S_O_FILES) $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = ../../shared/vectors/vectors.h

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/vectors.h: ../../shared/vectors/vectors.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE)
TMPINSTALL_FILES += $(PROJECT_INCLUDE)/bsp
TMPINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vectors.h

all-local: $(ARCH) $(TMPINSTALL_FILES) $(OBJS)

include $(top_srcdir)/../../../../../../automake/force-preinstall.am
include $(top_srcdir)/../../../../../../automake/local.am
