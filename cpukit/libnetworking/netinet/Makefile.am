##
## $Id$
##

include_netinetdir = $(includedir)/netinet

LIB = $(ARCH)/lib.a

C_FILES = if_ether.c igmp.c in.c in_cksum.c in_pcb.c in_proto.c in_rmx.c \
    ip_divert.c ip_fw.c ip_icmp.c ip_input.c ip_mroute.c ip_output.c \
    raw_ip.c tcp_debug.c tcp_input.c tcp_output.c tcp_subr.c tcp_timer.c \
    tcp_usrreq.c udp_usrreq.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

#
# Add local stuff here using +=
#

# Add -DFORWARD_PROTOCOL to enable UDP forwarding -- requires missing net/pf.h
AM_CPPFLAGS += -D_COMPILING_BSD_KERNEL_ -DKERNEL -DINET -DNFS -DDIAGNOSTIC \
    -DBOOTP_COMPAT

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(ARCH) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = if_ether.c igmp.c igmp_var.h in.c in_cksum.c in_cksum_i386.c \
    in_cksum_m68k.c in_cksum_powerpc.c in_pcb.c in_proto.c in_rmx.c \
    ip_divert.c ip_fw.c ip_icmp.c ip_input.c ip_mroute.c ip_output.c \
    raw_ip.c tcp_debug.c tcp_input.c tcp_output.c tcp_subr.c tcp_timer.c \
    tcp_usrreq.c udp_usrreq.c

include $(top_srcdir)/../automake/local.am
