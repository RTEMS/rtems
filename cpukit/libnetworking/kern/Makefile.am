##
## $Id$
##

LIB = $(ARCH)/lib.a

C_FILES = kern_subr.c uipc_domain.c uipc_mbuf.c uipc_socket.c uipc_socket2.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

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

all-local: $(ARCH) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = kern_subr.c uipc_domain.c uipc_mbuf.c uipc_socket.c \
    uipc_socket2.c

include $(top_srcdir)/../automake/local.am
