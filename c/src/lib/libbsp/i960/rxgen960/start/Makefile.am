##
## $Id$
##

S_FILES = rxgen_romld.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

OBJS = $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

bsplib_DATA = $(PROJECT_RELEASE)/lib/rxgen_romld.o

$(PROJECT_RELEASE)/lib/rxgen_romld$(LIB_VARIANT).o: $(ARCH)/rxgen_romld.o
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/rxgen_romld$(LIB_VARIANT).o

all-local: $(ARCH) $(OBJS) $(ARCH)/rxgen_romld.o $(TMPINSTALL_FILES)

.PRECIOUS: $(ARCH)/rxgen_romld.o

EXTRA_DIST = rxgen_romld.S

include $(top_srcdir)/../../../../../../automake/local.am
