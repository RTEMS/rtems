##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

SUBDIRS = . score clock timer sci delay

# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts
include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score

include_sh_HEADERS = include/sci.h include/sh7_pfc.h include/sh7_sci.h
include_rtems_score_HEADERS = include/ispsh7032.h include/iosh7032.h

$(PROJECT_INCLUDE)/sh:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/sh/%.h: include/%.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems/score/%.h: include/%.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_INCLUDE)/sh \
    $(include_sh_HEADERS:include/%.h=$(PROJECT_INCLUDE)/sh/%.h) \
    $(include_rtems_score_HEADERS:include/%.h=$(PROJECT_INCLUDE)/rtems/score/%.h)

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
