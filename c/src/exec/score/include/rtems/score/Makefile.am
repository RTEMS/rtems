##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtems_scoredir = $(includedir)/rtems/score

# We only build multiprocessing related files if HAS_MP was defined
MP_H_FILES = mpci.h mppkt.h objectmp.h threadmp.h

# H_FILES that get installed in the rtems/score subdirectoy
STD_H_FILES = address.h apiext.h bitfield.h chain.h context.h copyrt.h \
    coremsg.h coremutex.h coresem.h heap.h interr.h isr.h object.h \
    priority.h stack.h states.h sysstate.h thread.h threadq.h tod.h tqdata.h \
    userext.h watchdog.h wkspace.h

if HAS_MP
include_rtems_score_HEADERS = $(STD_H_FILES) $(MP_H_FILES) cpuopts.h
else
include_rtems_score_HEADERS = $(STD_H_FILES) cpuopts.h
endif

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%=$(PROJECT_INCLUDE)/rtems/score/%)

$(PROJECT_INCLUDE)/rtems/score:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

# make the cpu dependent options file
# FIXME: We are exporting a config-header here, which is a bad idea in
# general
cpuopts.h: cpuopts-tmp.h
	@echo "/* target cpu dependent options file */"        >$@
	@echo "/* automatically generated -- DO NOT EDIT!! */" >>$@
	@echo                                                  >>$@
	@echo "#ifndef __CPU_OPTIONS_h"                        >>$@
	@echo "#define __CPU_OPTIONS_h"                        >>$@
	@echo                                                  >>$@
	@cat cpuopts-tmp.h                                     >>$@
	@echo                                                  >>$@
	@echo "#endif"                                         >>$@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
