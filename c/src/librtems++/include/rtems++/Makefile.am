##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_CXX
H_PIECES = rtemsEvent.h rtemsInterrupt.h rtemsMessageQueue.h \
    rtemsSemaphore.h rtemsStatusCode.h rtemsTask.h rtemsTaskMode.h rtemsTimer.h

noinst_HEADERS = $(H_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems++ \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rtems++/%)

$(PROJECT_INCLUDE)/rtems++:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems++/%.h: %.h
	$(INSTALL_DATA) $< $@
endif

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
