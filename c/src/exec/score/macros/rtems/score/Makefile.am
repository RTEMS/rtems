##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

EXTRA_DIST = README

if MACROS
## We only build multiprocessing related files if HAS_MP was defined
MP_I_PIECES_yes_V = mppkt.inl objectmp.inl threadmp.inl
MP_I_PIECES = $(MP_I_PIECES_$(HAS_MP)_V)

I_PIECES = address.inl chain.inl coremsg.inl coremutex.inl coresem.inl heap.inl \
    isr.inl object.inl priority.inl stack.inl states.inl sysstate.inl \
    thread.inl tod.inl tqdata.inl userext.inl watchdog.inl wkspace.inl \
    $(MP_I_PIECES)

noinst_HEADERS = $(I_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE) \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE)/rtems/score:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/score/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
