##
## $Id$
##


include_rtems__dir = $(includedir)/rtems++

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems++:
	@$(mkinstalldirs) $@

if HAS_CXX
include_rtems___HEADERS = rtems++/rtemsEvent.h \
    rtems++/rtemsInterrupt.h rtems++/rtemsMessageQueue.h rtems++/rtemsSemaphore.h \
    rtems++/rtemsStatusCode.h rtems++/rtemsTask.h rtems++/rtemsTaskMode.h \
    rtems++/rtemsTimer.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems++ \
    $(include_rtems___HEADERS:%=$(PROJECT_INCLUDE)/%)

endif

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
