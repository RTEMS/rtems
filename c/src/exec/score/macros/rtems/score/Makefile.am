##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtems_scoredir = $(includedir)/rtems/score

## We only build multiprocessing related files if HAS_MP was defined
MP_I_FILES = mppkt.inl objectmp.inl threadmp.inl

STD_I_FILES = address.inl chain.inl coremsg.inl coremutex.inl coresem.inl \
    heap.inl isr.inl object.inl priority.inl stack.inl states.inl \
    sysstate.inl thread.inl tod.inl tqdata.inl userext.inl watchdog.inl \
    wkspace.inl

I_FILES = $(STD_I_FILES) $(MP_I_FILES)

if MACROS
if HAS_MP
include_rtems_score_HEADERS = $(STD_I_FILES) $(MP_I_FILES)
else
include_rtems_score_HEADERS = $(STD_I_FILES)
endif

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%=$(PROJECT_INCLUDE)/rtems/score/%)

$(PROJECT_INCLUDE)/rtems/score:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/score/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = $(I_FILES)

include $(top_srcdir)/../../../automake/local.am
