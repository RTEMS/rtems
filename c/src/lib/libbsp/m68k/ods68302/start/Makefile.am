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
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES) $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am
include $(top_srcdir)/../../bspstart.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS += $(CFLAGS_V)

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)

$(ARCH)/start.$(OBJEXT): $(OBJS)
	cp $(S_O_FILES) $(ARCH)/start.$(OBJEXT)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = cpuboot.c debugreset.S reset.S

${PROJECT_RELEASE}/lib/$(dirstamp):
	@$(mkdir_p) ${PROJECT_RELEASE}/lib
	@: > ${PROJECT_RELEASE}/lib/$(dirstamp)

${PROJECT_RELEASE}/lib/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start$(LIB_VARIANT).$(OBJEXT) ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/start$(LIB_VARIANT).$(OBJEXT)

include $(top_srcdir)/../../../../automake/local.am
