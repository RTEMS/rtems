##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES = no-ext.c no-io.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

SRCS = $(C_FILES)
OBJS = $(C_O_FILES)

PGMNAME = $(C_FILES:%.c=%)
PGMS = $(C_FILES:%.c=$(ARCH)/%.rel)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg

TMPINSTALL_FILES = \
$(PROJECT_RELEASE)/lib \
$(PGMNAME:%=$(PROJECT_RELEASE)/lib/%$(LIB_VARIANT).rel)

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS += $(CFLAGS_OS_V)
ASM4FLAGS += -I $(PROJECT_INCLUDE)/rtems

$(PROJECT_RELEASE)/lib:
	$(mkinstalldirs) $@
$(PROJECT_RELEASE)/lib/%$(LIB_VARIANT).rel: $(ARCH)/%.rel
	$(INSTALL_DATA) $< $@

all: ${ARCH} $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../automake/local.am
