##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.5
ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

include $(top_srcdir)/../../../../../../automake/multilib.am
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

include_HEADERS = asm.h
PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/ppc.h rtems/score/ppctypes.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%.h=$(PROJECT_INCLUDE)/%.h)

# $(PROJECT_RELEASE)/lib$(MULTISUBDIR)/rtems$(LIB_VARIANT).o: $(ARCH)/rtems.o
#	$(INSTALL_DATA) $< $@

# $(REL): $(rtems_cpu_rel_OBJECTS)
#	$(make-rel)


# TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib$(MULTISUBDIR)/rtems$(LIB_VARIANT).o

all-local: $(ARCH) $(PREINSTALL_FILES) $(rtems_cpu_rel_OBJECTS) $(REL) \
    $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
