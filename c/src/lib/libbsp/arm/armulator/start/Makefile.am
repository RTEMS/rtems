##
## $Id$
##

S_FILES = start.S
S_O_FILES = $(S_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

# From newlib
# Select which debug protocol is being used.
# ARM_RDP_MONITOR selects the Demon monitor.
# ARM_RDI_MONITOR selects the Angel monitor.
# If neither are defined, then hard coded defaults will be used
# to create the program's environment.
AM_CPPFLAGS += -DARM_RDI_MONITOR

install-data-local: $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT)
	@$(mkinstalldirs) $(DESTDIR)$(bsplibdir)
	$(INSTALL_DATA) $< $(DESTDIR)$(bsplibdir)

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start.$(OBJEXT)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(ARCH) $(OBJS) $(ARCH)/start.$(OBJEXT) $(TMPINSTALL_FILES)

.PRECIOUS: $(ARCH)/start.$(OBJEXT)

EXTRA_DIST = start.S

include $(top_srcdir)/../../../../../../automake/local.am
