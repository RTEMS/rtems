##
## $Id$
##


PGMNAME = dummy
PGM = $(ARCH)/$(PGMNAME).rel

C_FILES = dummy.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

$(PROJECT_RELEASE)/lib/$(PGMNAME)$(LIB_VARIANT).rel: $(PGM)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(PGMNAME)$(LIB_VARIANT).rel

all-local: $(ARCH) $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = README dummy.c

include $(top_srcdir)/../../../automake/local.am
