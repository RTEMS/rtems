##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.5
ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

SUBDIRS = rtems

include_HEADERS = asm.h

include $(top_srcdir)/../../../../../../automake/multilib.am
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

# $(PROJECT_RELEASE)/lib$(MULTISUBDIR)/rtems$(LIB_VARIANT).o: $(ARCH)/rtems.o
#	$(INSTALL_DATA) $< $@

# $(REL): $(rtems_cpu_rel_OBJECTS)
#	$(make-rel)

PREINSTALL_FILES = $(PROJECT_INCLUDE) $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

# TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib$(MULTISUBDIR)/rtems$(LIB_VARIANT).o

all-local: $(ARCH) $(PREINSTALL_FILES) $(rtems_cpu_rel_OBJECTS) $(REL) \
    $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
