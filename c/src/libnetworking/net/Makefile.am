## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

C_FILES = if.c if_ethersubr.c if_loop.c radix.c route.c rtsock.c raw_cb.c \
    raw_usrreq.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D_COMPILING_BSD_KERNEL_ -DKERNEL -DINET -DNFS -DDIAGNOSTIC \
    -DBOOTP_COMPAT

$(LIB): $(OBJS)
	$(make-library)

all: $(ARCH) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = bpf.h ethernet.h if.c if.h if_arp.h if_dl.h if_ethersubr.c \
    if_llc.h if_loop.c if_ppp.h if_types.h netisr.h ppp-comp.h ppp_defs.h \
    radix.c radix.h raw_cb.c raw_cb.h raw_usrreq.c route.c route.h rtsock.c

include $(top_srcdir)/../../../automake/local.am
