##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

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
include_rtems_score_HEADERS = \
  rtems/score/ppc.h \
  rtems/score/types.h
include_rtems_score_HEADERS += rtems/score/cpu.h

$(PROJECT_INCLUDE)/rtems/old-exceptions:
	$(mkinstalldirs) $@

include_rtems_old_exceptionsdir = $(includedir)/rtems/old-exceptions
include_rtems_old_exceptions_HEADERS = rtems/old-exceptions/cpu.h

$(PROJECT_INCLUDE)/rtems/new-exceptions:
	$(mkinstalldirs) $@

include_rtems_new_exceptionsdir = $(includedir)/rtems/new-exceptions
include_rtems_new_exceptions_HEADERS = rtems/new-exceptions/cpu.h


$(PROJECT_INCLUDE)/rtems/powerpc:
	$(mkinstalldirs) $@

include_rtems_powerpcdir = $(includedir)/rtems/powerpc
include_rtems_powerpc_HEADERS = rtems/powerpc/registers.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%.h=$(PROJECT_INCLUDE)/%.h)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/old-exceptions \
    $(include_rtems_old_exceptions_HEADERS:%.h=$(PROJECT_INCLUDE)/%.h)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/new-exceptions \
    $(include_rtems_new_exceptions_HEADERS:%.h=$(PROJECT_INCLUDE)/%.h)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/powerpc \
    $(include_rtems_powerpc_HEADERS:%.h=$(PROJECT_INCLUDE)/%.h)

# $(PROJECT_RELEASE)/lib$(MULTISUBDIR)/rtems$(LIB_VARIANT).o: $(ARCH)/rtems.o
#	$(INSTALL_DATA) $< $@

# $(REL): $(rtems_cpu_rel_OBJECTS)
#	$(make-rel)


# TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib$(MULTISUBDIR)/rtems$(LIB_VARIANT).o

all-local: $(ARCH) $(PREINSTALL_FILES) $(rtems_cpu_rel_OBJECTS) $(REL) \
    $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
