##
## $Id$
##

LIB = $(ARCH)/lib.a

C_FILES = kern_subr.c uipc_domain.c uipc_mbuf.c uipc_socket.c uipc_socket2.c
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

all-local: $(ARCH) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = kern_subr.c uipc_domain.c uipc_mbuf.c uipc_socket.c \
    uipc_socket2.c

include $(top_srcdir)/../automake/local.am
