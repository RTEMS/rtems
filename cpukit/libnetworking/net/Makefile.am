##
## $Id$
##


include_netdir = $(includedir)/net

LIB = $(ARCH)/lib.a

C_FILES = if.c if_ethersubr.c if_loop.c radix.c route.c rtsock.c raw_cb.c \
    raw_usrreq.c if_ppp.c ppp_tty.c pppcompress.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D_COMPILING_BSD_KERNEL_ -DKERNEL -DINET -DNFS -DDIAGNOSTIC \
    -DBOOTP_COMPAT

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(ARCH) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = if.c if_ethersubr.c if_loop.c radix.c raw_cb.c raw_usrreq.c \
    route.c rtsock.c

include $(top_srcdir)/../automake/local.am
