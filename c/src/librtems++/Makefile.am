##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include_rtems__dir = $(includedir)/rtems++

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems++:
	@$(mkinstalldirs) $@

if HAS_CXX
include_rtems___HEADERS = include/rtems++/rtemsEvent.h \
    include/rtems++/rtemsInterrupt.h include/rtems++/rtemsMessageQueue.h include/rtems++/rtemsSemaphore.h \
    include/rtems++/rtemsStatusCode.h include/rtems++/rtemsTask.h include/rtems++/rtemsTaskMode.h \
    include/rtems++/rtemsTimer.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems++ \
    $(include_rtems___HEADERS:include/%=$(PROJECT_INCLUDE)/%)

endif

LIB = ${ARCH}/librtems++.a

CC_FILES = src/rtemsEvent.cc src/rtemsInterrupt.cc src/rtemsMessageQueue.cc \
    src/rtemsSemaphore.cc src/rtemsStatusCode.cc src/rtemsTask.cc src/rtemsTimer.cc
CC_O_FILES = $(CC_FILES:src/%.cc=${ARCH}/%.$(OBJEXT))

SRCS = $(CC_FILES)
OBJS = $(CC_O_FILES)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/librtems++$(LIB_VARIANT).a

bsplib_DATA = $(LIB)

${ARCH}/%.o: src/%.cc
	test -d $(ARCH) || mkdir $(ARCH)
	${CXXCOMPILE} -o $@ -c $<

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_RELEASE)/lib/librtems++$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all-local: ${ARCH} $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
