##
## $Id$
##

S_FILES = rxgen_romld.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am
include $(top_srcdir)/../../bspstart.am

#
# (OPTIONAL) Add local stuff here using +=
#

project_lib_DATA = $(ARCH)/rxgen_romld$(LIB_VARIANT).$(OBJEXT)

all-local: $(ARCH)/rxgen_romld.$(OBJEXT) $(TMPINSTALL_FILES)

EXTRA_DIST = rxgen_romld.S

${PROJECT_RELEASE}/lib/$(dirstamp):
	@$(mkdir_p) ${PROJECT_RELEASE}/lib
	@: > ${PROJECT_RELEASE}/lib/$(dirstamp)

${PROJECT_RELEASE}/lib/rxgen_romld$(LIB_VARIANT).$(OBJEXT): $(ARCH)/rxgen_romld$(LIB_VARIANT).$(OBJEXT) ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/rxgen_romld$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/rxgen_romld$(LIB_VARIANT).$(OBJEXT)

include $(top_srcdir)/../../../../automake/local.am
