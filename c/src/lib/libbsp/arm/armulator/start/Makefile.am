##
## $Id$
##

S_FILES = start.S
S_O_FILES = $(S_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am
include $(top_srcdir)/../../bspstart.am

#
# (OPTIONAL) Add local stuff here using +=
#

# From newlib
# Select which debug protocol is being used.
# ARM_RDP_MONITOR selects the Demon monitor.
# ARM_RDI_MONITOR selects the Angel monitor.
# If neither are defined, then hard coded defaults will be used
# to create the program's environment.
AM_CPPFLAGS += -DARM_RDI_MONITOR

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = start.S

${PROJECT_RELEASE}/lib/$(dirstamp):
	@$(mkdir_p) ${PROJECT_RELEASE}/lib
	@: > ${PROJECT_RELEASE}/lib/$(dirstamp)

${PROJECT_RELEASE}/lib/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start$(LIB_VARIANT).$(OBJEXT) ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/start$(LIB_VARIANT).$(OBJEXT)

include $(top_srcdir)/../../../../automake/local.am
