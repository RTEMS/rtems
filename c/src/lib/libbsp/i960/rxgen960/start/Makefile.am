##
## $Id$
##

S_FILES = rxgen_romld.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

bsplib_DATA = $(PROJECT_RELEASE)/lib/rxgen_romld.$(OBJEXT)

$(PROJECT_RELEASE)/lib/rxgen_romld$(LIB_VARIANT).$(OBJEXT): $(ARCH)/rxgen_romld.$(OBJEXT)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += \
    $(PROJECT_RELEASE)/lib/rxgen_romld$(LIB_VARIANT).$(OBJEXT)

all-local: $(OBJS) $(ARCH)/rxgen_romld.$(OBJEXT) $(TMPINSTALL_FILES)

.PRECIOUS: $(ARCH)/rxgen_romld.$(OBJEXT)

EXTRA_DIST = rxgen_romld.S

include $(top_srcdir)/../../../../automake/local.am
