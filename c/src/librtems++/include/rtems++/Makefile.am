##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_CXX
include_rtems__dir = $(includedir)/rtems++
include_rtems___HEADERS = rtemsEvent.h rtemsInterrupt.h rtemsMessageQueue.h rtemsSemaphore.h \
    rtemsStatusCode.h rtemsTask.h rtemsTaskMode.h rtemsTimer.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems++ \
    $(include_rtems___HEADERS:%=$(PROJECT_INCLUDE)/rtems++/%)

$(PROJECT_INCLUDE)/rtems++:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems++/%.h: %.h
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
