##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if INLINE
## We only build multiprocessing related files if HAS_MP was defined
if HAS_MP
MP_I_FILES = mppkt.inl objectmp.inl threadmp.inl
endif

I_FILES = address.inl chain.inl coremsg.inl coremutex.inl coresem.inl \
    heap.inl isr.inl object.inl priority.inl stack.inl states.inl \
    sysstate.inl thread.inl tod.inl tqdata.inl userext.inl watchdog.inl \
    wkspace.inl $(MP_I_FILES)

noinst_HEADERS = $(I_FILES)

PREINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems/score \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rtems/score/%)

$(PROJECT_INCLUDE)/rtems/score:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/score/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-am: $(PREINSTALL_FILES)
debug-am: $(PREINSTALL_FILES)
profile-am: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
