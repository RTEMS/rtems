##
## $Id$
##

S_FILES = start.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

noinst_FILES = 80386ex.h

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

install-data-local: $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT)
	@$(mkinstalldirs) $(DESTDIR)$(bsplibdir)
	$(INSTALL_DATA) $< $(DESTDIR)$(bsplibdir)

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start.$(OBJEXT)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(ARCH) $(OBJS) $(TMPINSTALL_FILES)

EXTRA_DIST = 80386ex.h 80386ex.inc macros.inc start.S ts_1325.inc

include $(top_srcdir)/../../../../automake/local.am
