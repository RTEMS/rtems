##
## $Id$
##

S_FILES = start.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

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

all-local: $(OBJS) $(TMPINSTALL_FILES)

EXTRA_DIST = start.S start.ram start.rom

include $(top_srcdir)/../../../../automake/local.am
