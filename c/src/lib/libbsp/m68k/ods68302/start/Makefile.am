##
## $Id$
##

# FIXME: Better merge both files into one and use #ifdef GDB_MONITOR_ACTIVE
if ODS68302_DEBUG
RESET_S_FILES = debugreset.S
else
RESET_S_FILES = reset.S
endif

S_FILES = $(RESET_S_FILES)
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

OBJS = $(S_O_FILES) $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS += $(CFLAGS_V)

bsplib_DATA = $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o

$(ARCH)/start.o: $(OBJS)
	cp $(S_O_FILES) $(ARCH)/start.o

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o: $(ARCH)/start.o
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o

all-local: $(ARCH) $(OBJS) $(ARCH)/start.o $(TMPINSTALL_FILES)

.PRECIOUS: $(ARCH)/start.o

EXTRA_DIST = cpuboot.c debugreset.S reset.S

include $(top_srcdir)/../../../../../../automake/local.am
