##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/start

S_FILES = start.S rtems_crti.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#
bsplib_DATA =  $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT)
bsplib_DATA += $(PROJECT_RELEASE)/lib/rtems_crti.$(OBJEXT)

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start.$(OBJEXT)
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/rtems_crti.$(OBJEXT): $(ARCH)/rtems_crti.$(OBJEXT)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/rtems_crti.$(OBJEXT)

all-local: $(ARCH) $(OBJS) $(ARCH)/start.$(OBJEXT) $(ARCH)/rtems_crti.$(OBJEXT) $(TMPINSTALL_FILES)

$(OBJS): $(ARCH)

.PRECIOUS: $(ARCH)/start.$(OBJEXT) $(ARCH)/rtems_crti.$(OBJEXT)

include $(top_srcdir)/../../../../../../automake/local.am
