##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

ROOT_H_FILES = asm.h
RTEMS_SCORE_H_FILES = ppc.h ppctypes.h
noinst_HEADERS = $(ROOT_H_FILES) $(RTEMS_SCORE_H_FILES)

#
# (OPTIONAL) Add local stuff here using +=
#

PREINSTALL_FILES = \
$(PROJECT_INCLUDE) \
$(PROJECT_INCLUDE)/rtems/score \
$(ROOT_H_FILES:%.h=$(PROJECT_INCLUDE)/%.h) \
$(RTEMS_SCORE_H_FILES:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h)

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
