##
## $Id$
##

S_FILES = $(top_srcdir)/../shared/start.S
S_O_FILES = $(S_FILES:$(top_srcdir)/../shared/%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)

$(ARCH)/%$(LIB_VARIANT).$(OBJEXT): $(top_srcdir)/../shared/%.S $(ARCH)/$(dirstamp)
	${CCASCOMPILE} -DASM -o $@ -c $<

all-local: $(TMPINSTALL_FILES)

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

include $(top_srcdir)/../../../../automake/local.am
