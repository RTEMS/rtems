##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

## We only build multiprocessing related files if HAS_MP was defined
MP_H_FILES = eventmp.h mp.h msgmp.h partmp.h regionmp.h semmp.h signalmp.h \
    taskmp.h

STD_H_FILES = asr.h attr.h clock.h config.h dpmem.h event.h eventset.h \
    intr.h message.h modes.h options.h part.h ratemon.h region.h rtemsapi.h \
    sem.h signal.h status.h support.h tasks.h timer.h types.h

if HAS_MP
H_FILES = $(STD_H_FILES) $(MP_H_FILES)
else
H_FILES = $(STD_H_FILES)
endif

noinst_HEADERS = $(STD_H_FILES) $(MP_H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/rtems \
    $(H_FILES:%=$(PROJECT_INCLUDE)/rtems/rtems/%)

$(PROJECT_INCLUDE)/rtems/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
