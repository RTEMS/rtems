##
##  $Id$
##

LIB = ${ARCH}/librtems++.a

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

CC_FILES = rtemsEvent.cc rtemsInterrupt.cc rtemsMessageQueue.cc \
    rtemsSemaphore.cc rtemsStatusCode.cc rtemsTask.cc rtemsTimer.cc
CC_O_FILES = $(CC_FILES:%.cc=${ARCH}/%.$(OBJEXT))

SRCS = $(CC_FILES)
OBJS = $(CC_O_FILES)

AM_CFLAGS += $(LIBC_DEFINES)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/librtems++$(LIB_VARIANT).a

bsplib_DATA = $(LIB)

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_RELEASE)/lib/librtems++$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all-local: ${ARCH} $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
