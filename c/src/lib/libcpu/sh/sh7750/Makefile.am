##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

SUBDIRS = . score clock sci timer

# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts
include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score

include_sh_HEADERS = include/sh/sh4uart.h
include_rtems_score_HEADERS = \
include/rtems/score/sh7750_regs.h \
include/rtems/score/ipl.h \
include/rtems/score/iosh7750.h \
include/rtems/score/ispsh7750.h \
include/rtems/score/sh4_regs.h

$(PROJECT_INCLUDE)/sh:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = \
    $(PROJECT_INCLUDE)/sh \
    $(include_sh_HEADERS:include/%=$(PROJECT_INCLUDE)/%) \
    $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
