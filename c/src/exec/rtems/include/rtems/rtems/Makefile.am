#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

# We only build multiprocessing related files if HAS_MP was defined
MP_H_PIECES_yes_V = eventmp.h mp.h msgmp.h partmp.h regionmp.h semmp.h \
    signalmp.h taskmp.h
MP_H_PIECES = $(MP_H_PIECES_$(HAS_MP)_V)

H_PIECES = asr.h attr.h clock.h config.h dpmem.h event.h eventset.h intr.h \
    message.h modes.h options.h part.h ratemon.h region.h rtemsapi.h sem.h \
    signal.h status.h support.h tasks.h timer.h types.h $(MP_H_PIECES)

noinst_HEADERS = $(H_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems/rtems \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rtems/rtems/%)

$(PROJECT_INCLUDE)/rtems/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
