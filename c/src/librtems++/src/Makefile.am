##
##  $Id$
##


LIBNAME = librtems++
LIB = ${ARCH}/${LIBNAME}.a

CC_FILES = rtemsEvent.cc rtemsInterrupt.cc rtemsMessageQueue.cc \
    rtemsSemaphore.cc rtemsStatusCode.cc rtemsTask.cc rtemsTimer.cc
CC_O_FILES = $(CC_FILES:%.cc=${ARCH}/%.o)

SRCS = $(CC_FILES)
OBJS = $(CC_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

AM_CFLAGS += $(LIBC_DEFINES)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

$(LIB): ${OBJS}
	$(make-library)

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all-local: ${ARCH} $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
