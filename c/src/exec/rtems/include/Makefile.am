##
##  $Id$
##


$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

include_HEADERS = rtems.h

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

include_rtems_rtemsdir = $(includedir)/rtems/rtems

$(PROJECT_INCLUDE)/rtems/rtems:
	@$(mkinstalldirs) $@

## We only build multiprocessing related files if HAS_MP was defined
MP_H_FILES = rtems/rtems/eventmp.h rtems/rtems/mp.h rtems/rtems/msgmp.h \
    rtems/rtems/partmp.h rtems/rtems/regionmp.h rtems/rtems/semmp.h \
    rtems/rtems/signalmp.h rtems/rtems/taskmp.h

STD_H_FILES = rtems/rtems/asr.h rtems/rtems/attr.h rtems/rtems/cache.h \
    rtems/rtems/clock.h rtems/rtems/config.h rtems/rtems/dpmem.h \
    rtems/rtems/event.h rtems/rtems/eventset.h rtems/rtems/intr.h \
    rtems/rtems/message.h rtems/rtems/modes.h rtems/rtems/options.h \
    rtems/rtems/part.h rtems/rtems/ratemon.h rtems/rtems/region.h \
    rtems/rtems/rtemsapi.h rtems/rtems/sem.h rtems/rtems/signal.h \
    rtems/rtems/status.h rtems/rtems/support.h rtems/rtems/tasks.h \
    rtems/rtems/timer.h rtems/rtems/types.h

if HAS_MP
include_rtems_rtems_HEADERS = $(STD_H_FILES) $(MP_H_FILES)
else
include_rtems_rtems_HEADERS = $(STD_H_FILES)
endif

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/rtems \
    $(include_rtems_rtems_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
