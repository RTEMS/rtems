##
## $Id$
##

include_netinetdir = $(includedir)/netinet

LIB = $(ARCH)/lib.a

C_FILES = if_ether.c igmp.c in.c in_cksum.c in_pcb.c in_proto.c in_rmx.c \
    ip_divert.c ip_fw.c ip_icmp.c ip_input.c ip_mroute.c ip_output.c \
    raw_ip.c tcp_debug.c tcp_input.c tcp_output.c tcp_subr.c tcp_timer.c \
    tcp_usrreq.c udp_usrreq.c
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

EXTRA_DIST = if_ether.c igmp.c igmp_var.h in.c in_cksum.c in_cksum_i386.c \
    in_cksum_m68k.c in_cksum_powerpc.c in_pcb.c in_proto.c in_rmx.c \
    ip_divert.c ip_fw.c ip_icmp.c ip_input.c ip_mroute.c ip_output.c \
    raw_ip.c tcp_debug.c tcp_input.c tcp_output.c tcp_subr.c tcp_timer.c \
    tcp_usrreq.c udp_usrreq.c

include_netinet_HEADERS = icmp_var.h if_ether.h igmp.h igmp_var.h in.h in_pcb.h in_systm.h \
    in_var.h ip.h ip_fw.h ip_icmp.h ip_mroute.h ip_var.h tcp.h tcp_debug.h \
    tcp_fsm.h tcp_seq.h tcp_timer.h tcp_var.h tcpip.h udp.h udp_var.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/netinet \
    $(include_netinet_HEADERS:%=$(PROJECT_INCLUDE)/netinet/%)

$(PROJECT_INCLUDE)/netinet:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/netinet/%.h: %.h
	$(INSTALL_DATA) $< $@

include $(top_srcdir)/../automake/local.am
