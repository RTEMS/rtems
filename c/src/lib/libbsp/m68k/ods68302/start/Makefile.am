##
## $Id$
##

RESET_S_FILES_yes_V = debugreset.S
RESET_S_FILES_no_V = reset.S
RESET_S_FILES__V = $(RESET_S_FILES_no_V)
RESET_S_FILES = $(RESET_S_FILES_$(RTEMS_DEBUGGER)_V)

CFLAGS_no_V = -DGDB_MONITOR_ACTIVE
CFLAGS__V = $(CFLAGS_no_V)
CFLAGS_V = $(CFLAGS_$(RTEMS_DEBUGGER)_V)

C_FILES = cpuboot.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

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

bsplib_DATA = $(ARCH)/start.o

$(ARCH)/start.o: $(OBJS)
	$(make-rel)

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o: $(ARCH)/start.o
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o

all-local: $(ARCH) $(OBJS) $(ARCH)/start.o $(TMPINSTALL_FILES)

.PRECIOUS: $(ARCH)/start.o

EXTRA_DIST = cpuboot.c debugreset.S reset.S

include $(top_srcdir)/../../../../../../automake/local.am
