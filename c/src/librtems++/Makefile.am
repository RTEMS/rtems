##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

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

LIB = ${ARCH}/librtems++$(LIB_VARIANT).a

CC_FILES = src/rtemsEvent.cc src/rtemsInterrupt.cc src/rtemsMessageQueue.cc \
    src/rtemsSemaphore.cc src/rtemsStatusCode.cc src/rtemsTask.cc src/rtemsTimer.cc
OBJS = $(CC_FILES:src/%.cc=${ARCH}/%.$(OBJEXT))

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/librtems++$(LIB_VARIANT).a

install-data-local: $(LIB)
	@$(mkinstalldirs) $(DESTDIR)$(bsplibdir)
	$(INSTALL_DATA) $(LIB) $(DESTDIR)$(bsplibdir)

${ARCH}/%.o: src/%.cc
	test -d $(ARCH) || mkdir $(ARCH)
	${CXXCOMPILE} -o $@ -c $<

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_RELEASE)/lib/librtems++$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all-local: ${ARCH} $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
