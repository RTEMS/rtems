##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

${ARCH}/%.o: src/%.cc $(ARCH)/$(dirstamp)
	${CXXCOMPILE} -o $@ -c $<

if HAS_CXX
include_rtems__dir = $(includedir)/rtems++

include_rtems___HEADERS = include/rtems++/rtemsEvent.h \
    include/rtems++/rtemsInterrupt.h include/rtems++/rtemsMessageQueue.h \
    include/rtems++/rtemsSemaphore.h include/rtems++/rtemsStatusCode.h \
    include/rtems++/rtemsTask.h include/rtems++/rtemsTaskMode.h \
    include/rtems++/rtemsTimer.h

CC_FILES = src/rtemsEvent.cc src/rtemsInterrupt.cc src/rtemsMessageQueue.cc \
    src/rtemsSemaphore.cc src/rtemsStatusCode.cc src/rtemsTask.cc \
    src/rtemsTimer.cc
OBJS = $(CC_FILES:src/%.cc=${ARCH}/%.$(OBJEXT))

$(ARCH)/librtems++$(LIB_VARIANT).a: $(OBJS)
	$(make-library)

project_lib_DATA = $(ARCH)/librtems++$(LIB_VARIANT).a
endif

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

PREINSTALL_FILES =
TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

if HAS_CXX
$(PROJECT_INCLUDE)/rtems++/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems++
	@: > $(PROJECT_INCLUDE)/rtems++/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems++/$(dirstamp)

$(PROJECT_INCLUDE)/rtems++/rtemsEvent.h: include/rtems++/rtemsEvent.h $(PROJECT_INCLUDE)/rtems++/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems++/rtemsEvent.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems++/rtemsEvent.h

$(PROJECT_INCLUDE)/rtems++/rtemsInterrupt.h: include/rtems++/rtemsInterrupt.h $(PROJECT_INCLUDE)/rtems++/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems++/rtemsInterrupt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems++/rtemsInterrupt.h

$(PROJECT_INCLUDE)/rtems++/rtemsMessageQueue.h: include/rtems++/rtemsMessageQueue.h $(PROJECT_INCLUDE)/rtems++/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems++/rtemsMessageQueue.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems++/rtemsMessageQueue.h

$(PROJECT_INCLUDE)/rtems++/rtemsSemaphore.h: include/rtems++/rtemsSemaphore.h $(PROJECT_INCLUDE)/rtems++/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems++/rtemsSemaphore.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems++/rtemsSemaphore.h

$(PROJECT_INCLUDE)/rtems++/rtemsStatusCode.h: include/rtems++/rtemsStatusCode.h $(PROJECT_INCLUDE)/rtems++/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems++/rtemsStatusCode.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems++/rtemsStatusCode.h

$(PROJECT_INCLUDE)/rtems++/rtemsTask.h: include/rtems++/rtemsTask.h $(PROJECT_INCLUDE)/rtems++/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems++/rtemsTask.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems++/rtemsTask.h

$(PROJECT_INCLUDE)/rtems++/rtemsTaskMode.h: include/rtems++/rtemsTaskMode.h $(PROJECT_INCLUDE)/rtems++/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems++/rtemsTaskMode.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems++/rtemsTaskMode.h

$(PROJECT_INCLUDE)/rtems++/rtemsTimer.h: include/rtems++/rtemsTimer.h $(PROJECT_INCLUDE)/rtems++/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems++/rtemsTimer.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems++/rtemsTimer.h

$(PROJECT_LIB)/librtems++$(LIB_VARIANT).a: $(ARCH)/librtems++$(LIB_VARIANT).a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/librtems++$(LIB_VARIANT).a
TMPINSTALL_FILES += $(PROJECT_LIB)/librtems++$(LIB_VARIANT).a
endif

CLEANFILES = $(PREINSTALL_FILES)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
