##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = cpu.h mips.h mipstypes.h
noinst_HEADERS = $(H_FILES)

#
# (OPTIONAL) Add local stuff here using +=
#

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(H_FILES:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h)

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
