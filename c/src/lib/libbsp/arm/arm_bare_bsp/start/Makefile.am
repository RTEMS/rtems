## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGMS = $(ARCH)/start.o

S_FILES = start.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

OBJS = $(S_O_FILES)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

LINKCMDS = $(top_srcdir)/startup/linkcmds

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o: $(ARCH)/start.o
	$(INSTALL_DATA) $< $@


all: $(ARCH) $(OBJS) $(TMPINSTALL_FILES)

EXTRA_DIST = start.S

include $(top_srcdir)/../../../../../../automake/local.am
