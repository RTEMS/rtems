##
## $Id$
##


$(PROJECT_INCLUDE)/%: %
	$(INSTALL_DATA) $< $@

include_rtemsdir = $(includedir)/rtems

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@

include_rtems_HEADERS = rtems/debug.h rtems/system.h rtems/seterr.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/%)

include_rtems_scoredir = $(includedir)/rtems/score

$(PROJECT_INCLUDE)/rtems/score:
	@$(mkinstalldirs) $@

# We only build multiprocessing related files if HAS_MP was defined
MP_H_FILES = rtems/score/mpci.h rtems/score/mppkt.h rtems/score/objectmp.h \
    rtems/score/threadmp.h

# H_FILES that get installed in the rtems/score subdirectoy
STD_H_FILES = rtems/score/address.h rtems/score/apiext.h \
    rtems/score/apimutex.h \
    rtems/score/bitfield.h rtems/score/chain.h rtems/score/context.h \
    rtems/score/copyrt.h rtems/score/coremsg.h rtems/score/coremutex.h \
    rtems/score/coresem.h rtems/score/heap.h rtems/score/interr.h \
    rtems/score/isr.h rtems/score/object.h rtems/score/priority.h \
    rtems/score/stack.h rtems/score/states.h rtems/score/sysstate.h \
    rtems/score/thread.h rtems/score/threadq.h rtems/score/tod.h \
    rtems/score/tqdata.h rtems/score/userext.h rtems/score/watchdog.h \
    rtems/score/wkspace.h

if HAS_MP
include_rtems_score_HEADERS = $(STD_H_FILES) $(MP_H_FILES) rtems/score/cpuopts.h
else
include_rtems_score_HEADERS = $(STD_H_FILES) rtems/score/cpuopts.h
endif

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%=$(PROJECT_INCLUDE)/%)

# make the cpu dependent options file
# FIXME: We are exporting a config-header here, which is a bad idea in
# general
rtems/score/cpuopts.h: rtems/score/cpuopts-tmp.h
	@echo "/* target cpu dependent options file */"        >$@
	@echo "/* automatically generated -- DO NOT EDIT!! */" >>$@
	@echo                                                  >>$@
	@echo "#ifndef __CPU_OPTIONS_h"                        >>$@
	@echo "#define __CPU_OPTIONS_h"                        >>$@
	@echo                                                  >>$@
	@cat rtems/score/cpuopts-tmp.h | sed -e '/.*PACKAGE.*/d' >>$@
	@echo                                                  >>$@
	@echo "#endif"                                         >>$@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/automake/local.am
