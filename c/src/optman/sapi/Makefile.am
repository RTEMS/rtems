##
##  $Id$
##


C_FILES = no-ext.c no-io.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

PGMS = $(C_FILES:%.c=$(ARCH)/%.rel)

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

bsplib_DATA = $(PGMS)

TMPINSTALL_FILES += \
    $(C_FILES:%.c=$(PROJECT_RELEASE)/lib/%$(LIB_VARIANT).rel)

$(PROJECT_RELEASE)/lib$(MULTISUBDIR)/%$(LIB_VARIANT).rel: $(ARCH)/%.rel
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

all-local: ${ARCH} $(PGMS) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGMS)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../automake/local.am
