##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = librpc
LIB = ${ARCH}/${LIBNAME}.a

# C and C++ source names, if any, go here -- minus the .c or .cc
C_FILES = rpc_callmsg.c rpc_prot.c svc.c svc_auth.c svc_udp.c xdr.c \
    xdr_array.c xdr_mem.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

SRCS = $(C_FILES)
OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/lib.am

AM_CFLAGS += $(LIBC_DEFINES)

TMPINSTALL_FILES += \
$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

$(LIB): ${OBJS}
	$(make-library)

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all: ${ARCH} $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../automake/local.am
