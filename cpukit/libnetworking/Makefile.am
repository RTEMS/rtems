##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

# Add -DFORWARD_PROTOCOL to enable UDP forwarding -- requires missing net/pf.h
AM_CPPFLAGS += -D_COMPILING_BSD_KERNEL_ -DKERNEL -DINET -DNFS -DDIAGNOSTIC \
    -DBOOTP_COMPAT

LIB = $(ARCH)/libnetworking.a

SUBDIRS = . lib libc
SUBDIRS += wrapup

if HAS_NETWORKING
## poll.h is not in the list because RTEMS does not have poll()
## and installing poll.h confuses autoconf.
include_HEADERS = \
    bpfilter.h loop.h netdb.h opt_ipfw.h opt_mrouting.h \
    opt_tcpdebug.h resolv.h syslog.h memory.h

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
  $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

# arpa

include_arpadir = $(includedir)/arpa

include_arpa_HEADERS = arpa/ftp.h arpa/inet.h arpa/nameser.h \
    arpa/nameser_compat.h arpa/telnet.h

$(PROJECT_INCLUDE)/arpa:
	@$(mkinstalldirs) $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa \
    $(include_arpa_HEADERS:%=$(PROJECT_INCLUDE)/%)

## kern

kern_C_FILES = kern/kern_mib.c kern/kern_subr.c kern/kern_sysctl.c \
   kern/uipc_domain.c kern/uipc_mbuf.c kern/uipc_socket.c kern/uipc_socket2.c
OBJS = $(kern_C_FILES:kern/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): kern/%.c
	${COMPILE} -o $@ -c $<

EXTRA_DIST = $(kern_C_FILES)

## machine

include_machinedir = $(includedir)/machine

include_machine_HEADERS = machine/conf.h machine/cpu.h machine/cpufunc.h \
    machine/endian.h machine/in_cksum.h machine/vmparam.h

$(PROJECT_INCLUDE)/machine:
	@$(mkinstalldirs) $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine \
    $(include_machine_HEADERS:%=$(PROJECT_INCLUDE)/%)

## net

include_netdir = $(includedir)/net

include_net_HEADERS = net/bpf.h net/ethernet.h net/if.h net/if_arp.h \
    net/if_dl.h net/if_llc.h net/if_ppp.h net/if_types.h net/netisr.h \
    net/ppp-comp.h net/ppp_defs.h net/radix.h net/raw_cb.h net/route.h \
    net/if_pppvar.h net/pppcompress.h

$(PROJECT_INCLUDE)/net:
	@$(mkinstalldirs) $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/net \
    $(include_net_HEADERS:%=$(PROJECT_INCLUDE)/%)

net_C_FILES = net/if.c net/if_ethersubr.c net/if_loop.c net/radix.c \
    net/route.c net/rtsock.c net/raw_cb.c net/raw_usrreq.c net/if_ppp.c \
    net/ppp_tty.c net/pppcompress.c
OBJS += $(net_C_FILES:net/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): net/%.c
	${COMPILE} -o $@ -c $<

EXTRA_DIST += $(net_C_FILES)

## netinet

include_netinetdir = $(includedir)/netinet

include_netinet_HEADERS = netinet/icmp_var.h netinet/if_ether.h netinet/igmp.h \
    netinet/igmp_var.h netinet/in.h netinet/in_pcb.h netinet/in_systm.h \
    netinet/in_var.h netinet/ip.h netinet/ip_fw.h netinet/ip_icmp.h \
    netinet/ip_mroute.h netinet/ip_var.h netinet/tcp.h netinet/tcp_debug.h \
    netinet/tcp_fsm.h netinet/tcp_seq.h netinet/tcp_timer.h netinet/tcp_var.h \
    netinet/tcpip.h netinet/udp.h netinet/udp_var.h

$(PROJECT_INCLUDE)/netinet:
	@$(mkinstalldirs) $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet \
    $(include_netinet_HEADERS:%=$(PROJECT_INCLUDE)/%)

netinet_C_FILES = netinet/if_ether.c netinet/igmp.c netinet/in.c \
    netinet/in_cksum.c netinet/in_pcb.c netinet/in_proto.c netinet/in_rmx.c \
    netinet/ip_divert.c netinet/ip_fw.c netinet/ip_icmp.c netinet/ip_input.c \
    netinet/ip_mroute.c netinet/ip_output.c netinet/raw_ip.c \
    netinet/tcp_debug.c netinet/tcp_input.c netinet/tcp_output.c \
    netinet/tcp_subr.c netinet/tcp_timer.c netinet/tcp_usrreq.c \
    netinet/udp_usrreq.c
OBJS += $(netinet_C_FILES:netinet/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): netinet/%.c
	${COMPILE} -o $@ -c $<

EXTRA_DIST += $(netinet_C_FILES)

## nfs

include_nfsdir = $(includedir)/nfs

include_nfs_HEADERS = nfs/krpc.h nfs/nfs.h nfs/nfsdiskless.h nfs/nfsproto.h \
    nfs/rpcv2.h nfs/xdr_subs.h

$(PROJECT_INCLUDE)/nfs:
	@$(mkinstalldirs) $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs \
    $(include_nfs_HEADERS:%=$(PROJECT_INCLUDE)/%)

nfs_C_FILES = nfs/bootp_subr.c
OBJS += $(nfs_C_FILES:nfs/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): nfs/%.c
	${COMPILE} -o $@ -c $<

EXTRA_DIST += $(nfs_C_FILES)

## rtems

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = rtems/rtems_bsdnet.h rtems/rtems_bsdnet_internal.h \
    rtems/tftp.h rtems/ftpfs.h rtems/mkrootfs.h

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/%)

rtems_C_FILES = rtems/sghostname.c rtems/issetugid.c rtems/rtems_glue.c rtems/rtems_syscall.c \
    rtems/rtems_bootp.c rtems/rtems_showmbuf.c rtems/rtems_showroute.c rtems/rtems_showifstat.c \
    rtems/rtems_showipstat.c rtems/rtems_showicmpstat.c rtems/rtems_showtcpstat.c \
    rtems/rtems_showudpstat.c rtems/rtems_select.c rtems/mkrootfs.c \
    rtems/rtems_bsdnet_malloc_starvation.c
OBJS += $(rtems_C_FILES:rtems/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): rtems/%.c
	${COMPILE} -o $@ -c $<

EXTRA_DIST += $(rtems_C_FILES)

## sys

include_sysdir = $(includedir)/sys

include_sys_HEADERS = sys/buf.h sys/callout.h sys/conf.h sys/domain.h \
    sys/kernel.h sys/libkern.h sys/linker_set.h sys/malloc.h sys/mbuf.h sys/mount.h \
    sys/proc.h sys/protosw.h sys/queue.h sys/reboot.h sys/resourcevar.h \
    sys/rtprio.h sys/select.h sys/signalvar.h sys/socket.h sys/socketvar.h \
    sys/sysctl.h sys/syslog.h sys/systm.h sys/ttydefaults.h sys/ucred.h \
    sys/uio.h sys/un.h

$(PROJECT_INCLUDE)/sys:
	@$(mkinstalldirs) $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys \
    $(include_sys_HEADERS:%=$(PROJECT_INCLUDE)/%)

# vm

include_vmdir = $(includedir)/vm

include_vm_HEADERS = vm/vm.h vm/vm_extern.h vm/vm_kern.h vm/vm_param.h

$(PROJECT_INCLUDE)/vm:
	@$(mkinstalldirs) $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/vm \
    $(include_vm_HEADERS:%=$(PROJECT_INCLUDE)/%)

# ---

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(ARCH) $(LIB)

endif

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
