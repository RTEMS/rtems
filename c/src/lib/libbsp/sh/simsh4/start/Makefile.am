##
## $Id$
##

S_FILES = start.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

bsplib_DATA = $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o: $(ARCH)/start.o
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o

all-local: $(ARCH) $(OBJS) $(TMPINSTALL_FILES)

EXTRA_DIST = start.S trap.S

include $(top_srcdir)/../../../../../../automake/local.am
