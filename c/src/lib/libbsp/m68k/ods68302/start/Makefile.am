##
## $Id$
##

# FIXME: Better merge both files into one and use #ifdef GDB_MONITOR_ACTIVE
if ODS68302_DEBUG
RESET_S_FILES = debugreset.S
else
RESET_S_FILES = reset.S
endif

S_FILES = $(RESET_S_FILES)
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES) $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS += $(CFLAGS_V)

install-data-local: $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT)
	@$(mkinstalldirs) $(DESTDIR)$(bsplibdir)
	$(INSTALL_DATA) $< $(DESTDIR)$(bsplibdir)

$(ARCH)/start.$(OBJEXT): $(OBJS)
	cp $(S_O_FILES) $(ARCH)/start.$(OBJEXT)

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start.$(OBJEXT)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(ARCH) $(OBJS) $(ARCH)/start.$(OBJEXT) $(TMPINSTALL_FILES)

.PRECIOUS: $(ARCH)/start.$(OBJEXT)

EXTRA_DIST = cpuboot.c debugreset.S reset.S

include $(top_srcdir)/../../../../automake/local.am
