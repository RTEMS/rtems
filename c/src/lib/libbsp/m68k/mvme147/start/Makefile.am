##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared

S_FILES = start.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am
include $(top_srcdir)/../../bspstart.am

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(TMPINSTALL_FILES)

${PROJECT_RELEASE}/lib/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start$(LIB_VARIANT).$(OBJEXT) ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/start$(LIB_VARIANT).$(OBJEXT)

include $(top_srcdir)/../../../../automake/local.am
