##
## $Id$
##


PGM = $(ARCH)/rxgen_romld.o

S_FILES = rxgen_romld.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

OBJS = $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PROJECT_RELEASE)/lib/rxgen_romld$(LIB_VARIANT).o: $(PGM)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/rxgen_romld$(LIB_VARIANT).o

all-local: $(ARCH) $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = rxgen_romld.S

include $(top_srcdir)/../../../../../../automake/local.am
