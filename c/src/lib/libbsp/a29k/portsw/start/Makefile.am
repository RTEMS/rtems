##
## $Id$
##

S_FILES = start.S register.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

install-data-local: $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT)
	@$(mkinstalldirs) $(DESTDIR)$(bsplibdir)
	$(INSTALL_DATA) $< $(DESTDIR)$(bsplibdir)

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start.$(OBJEXT)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(ARCH) $(OBJS) $(ARCH)/start.$(OBJEXT) $(TMPINSTALL_FILES)

.PRECIOUS: $(ARCH)/start.$(OBJEXT)

EXTRA_DIST = amd.ah start.S pswmacro.ah register.S register.ah

include $(top_srcdir)/../../../../../../automake/local.am
