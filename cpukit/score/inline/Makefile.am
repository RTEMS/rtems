##
## $Id$
##


$(PROJECT_INCLUDE)/%: %
	$(INSTALL_DATA) $< $@

include_rtems_scoredir = $(includedir)/rtems/score

$(PROJECT_INCLUDE)/rtems/score:
	@$(mkinstalldirs) $@

## We only build multiprocessing related files if HAS_MP was defined
MP_I_FILES = rtems/score/mppkt.inl rtems/score/objectmp.inl rtems/score/threadmp.inl

STD_I_FILES = rtems/score/address.inl rtems/score/chain.inl \
    rtems/score/coremsg.inl rtems/score/coremutex.inl rtems/score/coresem.inl \
    rtems/score/heap.inl rtems/score/isr.inl rtems/score/object.inl \
    rtems/score/priority.inl rtems/score/stack.inl rtems/score/states.inl \
    rtems/score/sysstate.inl rtems/score/thread.inl rtems/score/tod.inl \
    rtems/score/tqdata.inl rtems/score/userext.inl rtems/score/watchdog.inl \
    rtems/score/wkspace.inl

if HAS_MP
I_FILES = $(STD_I_FILES) $(MP_I_FILES)
else
I_FILES = $(STD_I_FILES)
endif

if INLINE
include_rtems_score_HEADERS = $(I_FILES)

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%=$(PROJECT_INCLUDE)/%)

endif

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = $(STD_I_FILES) $(MP_I_FILES)

include $(top_srcdir)/../../../automake/local.am
