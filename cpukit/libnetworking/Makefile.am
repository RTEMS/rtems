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

if HAS_NETWORKING
## poll.h is not in the list because RTEMS does not have poll()
## and installing poll.h confuses autoconf.
include_HEADERS = bpfilter.h loop.h netdb.h opt_ipfw.h opt_mrouting.h \
    opt_tcpdebug.h resolv.h syslog.h memory.h

# arpa

include_arpadir = $(includedir)/arpa

include_arpa_HEADERS = arpa/ftp.h arpa/inet.h arpa/nameser.h \
    arpa/nameser_compat.h arpa/telnet.h

## kern

kern_C_FILES = kern/kern_mib.c kern/kern_subr.c kern/kern_sysctl.c \
    kern/uipc_domain.c kern/uipc_mbuf.c kern/uipc_socket.c \
    kern/uipc_socket2.c
OBJS = $(kern_C_FILES:kern/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): kern/%.c
	${COMPILE} -o $@ -c $<

EXTRA_DIST = $(kern_C_FILES)

## machine

include_machinedir = $(includedir)/machine

include_machine_HEADERS = machine/conf.h machine/cpu.h machine/cpufunc.h \
    machine/endian.h machine/in_cksum.h machine/vmparam.h

## net

include_netdir = $(includedir)/net

include_net_HEADERS = net/bpf.h net/ethernet.h net/if.h net/if_arp.h \
    net/if_dl.h net/if_llc.h net/if_ppp.h net/if_types.h net/netisr.h \
    net/ppp-comp.h net/ppp_defs.h net/radix.h net/raw_cb.h net/route.h \
    net/if_pppvar.h net/pppcompress.h

net_C_FILES = net/if.c net/if_ethersubr.c net/if_loop.c net/radix.c \
    net/route.c net/rtsock.c net/raw_cb.c net/raw_usrreq.c net/if_ppp.c \
    net/ppp_tty.c net/pppcompress.c
OBJS += $(net_C_FILES:net/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): net/%.c
	${COMPILE} -o $@ -c $<

EXTRA_DIST += $(net_C_FILES)

## netinet

include_netinetdir = $(includedir)/netinet

include_netinet_HEADERS = netinet/icmp_var.h netinet/if_ether.h \
    netinet/igmp.h netinet/igmp_var.h netinet/in.h netinet/in_pcb.h \
    netinet/in_systm.h netinet/in_var.h netinet/ip.h netinet/ip_fw.h \
    netinet/ip_icmp.h netinet/ip_mroute.h netinet/ip_var.h netinet/tcp.h \
    netinet/tcp_debug.h netinet/tcp_fsm.h netinet/tcp_seq.h \
    netinet/tcp_timer.h netinet/tcp_var.h netinet/tcpip.h netinet/udp.h \
    netinet/udp_var.h

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

nfs_C_FILES = nfs/bootp_subr.c
OBJS += $(nfs_C_FILES:nfs/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): nfs/%.c
	${COMPILE} -o $@ -c $<

EXTRA_DIST += $(nfs_C_FILES)

## rtems

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = rtems/rtems_bsdnet.h rtems/rtems_bsdnet_internal.h \
    rtems/tftp.h rtems/ftpfs.h rtems/mkrootfs.h

rtems_C_FILES = rtems/sghostname.c rtems/issetugid.c rtems/rtems_glue.c \
    rtems/rtems_syscall.c rtems/rtems_bootp.c rtems/rtems_showmbuf.c \
    rtems/rtems_showroute.c rtems/rtems_showifstat.c \
    rtems/rtems_showipstat.c rtems/rtems_showicmpstat.c \
    rtems/rtems_showtcpstat.c rtems/rtems_showudpstat.c rtems/rtems_select.c \
    rtems/mkrootfs.c rtems/rtems_bsdnet_malloc_starvation.c
OBJS += $(rtems_C_FILES:rtems/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): rtems/%.c
	${COMPILE} -o $@ -c $<

EXTRA_DIST += $(rtems_C_FILES)

## sys

include_sysdir = $(includedir)/sys

include_sys_HEADERS = sys/buf.h sys/callout.h sys/conf.h sys/domain.h \
    sys/kernel.h sys/libkern.h sys/linker_set.h sys/malloc.h sys/mbuf.h \
    sys/mount.h sys/proc.h sys/protosw.h sys/queue.h sys/reboot.h \
    sys/resourcevar.h sys/rtprio.h sys/select.h sys/signalvar.h sys/socket.h \
    sys/socketvar.h sys/sysctl.h sys/syslog.h sys/systm.h sys/ttydefaults.h \
    sys/ucred.h sys/uio.h sys/un.h

# vm

include_vmdir = $(includedir)/vm

include_vm_HEADERS = vm/vm.h vm/vm_extern.h vm/vm_kern.h vm/vm_param.h

# ---

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(ARCH) $(LIB)
endif

PREINSTALL_FILES =

if HAS_NETWORKING
$(PROJECT_INCLUDE)/bpfilter.h: bpfilter.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bpfilter.h

$(PROJECT_INCLUDE)/loop.h: loop.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/loop.h

$(PROJECT_INCLUDE)/netdb.h: netdb.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netdb.h

$(PROJECT_INCLUDE)/opt_ipfw.h: opt_ipfw.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/opt_ipfw.h

$(PROJECT_INCLUDE)/opt_mrouting.h: opt_mrouting.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/opt_mrouting.h

$(PROJECT_INCLUDE)/opt_tcpdebug.h: opt_tcpdebug.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/opt_tcpdebug.h

$(PROJECT_INCLUDE)/resolv.h: resolv.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/resolv.h

$(PROJECT_INCLUDE)/syslog.h: syslog.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/syslog.h

$(PROJECT_INCLUDE)/memory.h: memory.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/memory.h

$(PROJECT_INCLUDE)/arpa:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/arpa
PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa

$(PROJECT_INCLUDE)/arpa/ftp.h: arpa/ftp.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa/ftp.h

$(PROJECT_INCLUDE)/arpa/inet.h: arpa/inet.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa/inet.h

$(PROJECT_INCLUDE)/arpa/nameser.h: arpa/nameser.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa/nameser.h

$(PROJECT_INCLUDE)/arpa/nameser_compat.h: arpa/nameser_compat.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa/nameser_compat.h

$(PROJECT_INCLUDE)/arpa/telnet.h: arpa/telnet.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa/telnet.h

$(PROJECT_INCLUDE)/machine:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/machine
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine

$(PROJECT_INCLUDE)/machine/conf.h: machine/conf.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine/conf.h

$(PROJECT_INCLUDE)/machine/cpu.h: machine/cpu.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine/cpu.h

$(PROJECT_INCLUDE)/machine/cpufunc.h: machine/cpufunc.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine/cpufunc.h

$(PROJECT_INCLUDE)/machine/endian.h: machine/endian.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine/endian.h

$(PROJECT_INCLUDE)/machine/in_cksum.h: machine/in_cksum.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine/in_cksum.h

$(PROJECT_INCLUDE)/machine/vmparam.h: machine/vmparam.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine/vmparam.h

$(PROJECT_INCLUDE)/net:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/net
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net

$(PROJECT_INCLUDE)/net/bpf.h: net/bpf.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/bpf.h

$(PROJECT_INCLUDE)/net/ethernet.h: net/ethernet.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/ethernet.h

$(PROJECT_INCLUDE)/net/if.h: net/if.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if.h

$(PROJECT_INCLUDE)/net/if_arp.h: net/if_arp.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if_arp.h

$(PROJECT_INCLUDE)/net/if_dl.h: net/if_dl.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if_dl.h

$(PROJECT_INCLUDE)/net/if_llc.h: net/if_llc.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if_llc.h

$(PROJECT_INCLUDE)/net/if_ppp.h: net/if_ppp.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if_ppp.h

$(PROJECT_INCLUDE)/net/if_types.h: net/if_types.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if_types.h

$(PROJECT_INCLUDE)/net/netisr.h: net/netisr.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/netisr.h

$(PROJECT_INCLUDE)/net/ppp-comp.h: net/ppp-comp.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/ppp-comp.h

$(PROJECT_INCLUDE)/net/ppp_defs.h: net/ppp_defs.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/ppp_defs.h

$(PROJECT_INCLUDE)/net/radix.h: net/radix.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/radix.h

$(PROJECT_INCLUDE)/net/raw_cb.h: net/raw_cb.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/raw_cb.h

$(PROJECT_INCLUDE)/net/route.h: net/route.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/route.h

$(PROJECT_INCLUDE)/net/if_pppvar.h: net/if_pppvar.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if_pppvar.h

$(PROJECT_INCLUDE)/net/pppcompress.h: net/pppcompress.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/pppcompress.h

$(PROJECT_INCLUDE)/netinet:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/netinet
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet

$(PROJECT_INCLUDE)/netinet/icmp_var.h: netinet/icmp_var.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/icmp_var.h

$(PROJECT_INCLUDE)/netinet/if_ether.h: netinet/if_ether.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/if_ether.h

$(PROJECT_INCLUDE)/netinet/igmp.h: netinet/igmp.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/igmp.h

$(PROJECT_INCLUDE)/netinet/igmp_var.h: netinet/igmp_var.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/igmp_var.h

$(PROJECT_INCLUDE)/netinet/in.h: netinet/in.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/in.h

$(PROJECT_INCLUDE)/netinet/in_pcb.h: netinet/in_pcb.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/in_pcb.h

$(PROJECT_INCLUDE)/netinet/in_systm.h: netinet/in_systm.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/in_systm.h

$(PROJECT_INCLUDE)/netinet/in_var.h: netinet/in_var.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/in_var.h

$(PROJECT_INCLUDE)/netinet/ip.h: netinet/ip.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/ip.h

$(PROJECT_INCLUDE)/netinet/ip_fw.h: netinet/ip_fw.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/ip_fw.h

$(PROJECT_INCLUDE)/netinet/ip_icmp.h: netinet/ip_icmp.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/ip_icmp.h

$(PROJECT_INCLUDE)/netinet/ip_mroute.h: netinet/ip_mroute.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/ip_mroute.h

$(PROJECT_INCLUDE)/netinet/ip_var.h: netinet/ip_var.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/ip_var.h

$(PROJECT_INCLUDE)/netinet/tcp.h: netinet/tcp.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcp.h

$(PROJECT_INCLUDE)/netinet/tcp_debug.h: netinet/tcp_debug.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcp_debug.h

$(PROJECT_INCLUDE)/netinet/tcp_fsm.h: netinet/tcp_fsm.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcp_fsm.h

$(PROJECT_INCLUDE)/netinet/tcp_seq.h: netinet/tcp_seq.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcp_seq.h

$(PROJECT_INCLUDE)/netinet/tcp_timer.h: netinet/tcp_timer.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcp_timer.h

$(PROJECT_INCLUDE)/netinet/tcp_var.h: netinet/tcp_var.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcp_var.h

$(PROJECT_INCLUDE)/netinet/tcpip.h: netinet/tcpip.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcpip.h

$(PROJECT_INCLUDE)/netinet/udp.h: netinet/udp.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/udp.h

$(PROJECT_INCLUDE)/netinet/udp_var.h: netinet/udp_var.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/udp_var.h

$(PROJECT_INCLUDE)/nfs:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/nfs
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs

$(PROJECT_INCLUDE)/nfs/krpc.h: nfs/krpc.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs/krpc.h

$(PROJECT_INCLUDE)/nfs/nfs.h: nfs/nfs.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs/nfs.h

$(PROJECT_INCLUDE)/nfs/nfsdiskless.h: nfs/nfsdiskless.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs/nfsdiskless.h

$(PROJECT_INCLUDE)/nfs/nfsproto.h: nfs/nfsproto.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs/nfsproto.h

$(PROJECT_INCLUDE)/nfs/rpcv2.h: nfs/rpcv2.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs/rpcv2.h

$(PROJECT_INCLUDE)/nfs/xdr_subs.h: nfs/xdr_subs.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs/xdr_subs.h

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rtems
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems

$(PROJECT_INCLUDE)/rtems/rtems_bsdnet.h: rtems/rtems_bsdnet.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/rtems_bsdnet.h

$(PROJECT_INCLUDE)/rtems/rtems_bsdnet_internal.h: rtems/rtems_bsdnet_internal.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/rtems_bsdnet_internal.h

$(PROJECT_INCLUDE)/rtems/tftp.h: rtems/tftp.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/tftp.h

$(PROJECT_INCLUDE)/rtems/ftpfs.h: rtems/ftpfs.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/ftpfs.h

$(PROJECT_INCLUDE)/rtems/mkrootfs.h: rtems/mkrootfs.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/mkrootfs.h

$(PROJECT_INCLUDE)/sys:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/sys
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys

$(PROJECT_INCLUDE)/sys/buf.h: sys/buf.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/buf.h

$(PROJECT_INCLUDE)/sys/callout.h: sys/callout.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/callout.h

$(PROJECT_INCLUDE)/sys/conf.h: sys/conf.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/conf.h

$(PROJECT_INCLUDE)/sys/domain.h: sys/domain.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/domain.h

$(PROJECT_INCLUDE)/sys/kernel.h: sys/kernel.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/kernel.h

$(PROJECT_INCLUDE)/sys/libkern.h: sys/libkern.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/libkern.h

$(PROJECT_INCLUDE)/sys/linker_set.h: sys/linker_set.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/linker_set.h

$(PROJECT_INCLUDE)/sys/malloc.h: sys/malloc.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/malloc.h

$(PROJECT_INCLUDE)/sys/mbuf.h: sys/mbuf.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/mbuf.h

$(PROJECT_INCLUDE)/sys/mount.h: sys/mount.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/mount.h

$(PROJECT_INCLUDE)/sys/proc.h: sys/proc.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/proc.h

$(PROJECT_INCLUDE)/sys/protosw.h: sys/protosw.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/protosw.h

$(PROJECT_INCLUDE)/sys/queue.h: sys/queue.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/queue.h

$(PROJECT_INCLUDE)/sys/reboot.h: sys/reboot.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/reboot.h

$(PROJECT_INCLUDE)/sys/resourcevar.h: sys/resourcevar.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/resourcevar.h

$(PROJECT_INCLUDE)/sys/rtprio.h: sys/rtprio.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/rtprio.h

$(PROJECT_INCLUDE)/sys/select.h: sys/select.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/select.h

$(PROJECT_INCLUDE)/sys/signalvar.h: sys/signalvar.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/signalvar.h

$(PROJECT_INCLUDE)/sys/socket.h: sys/socket.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/socket.h

$(PROJECT_INCLUDE)/sys/socketvar.h: sys/socketvar.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/socketvar.h

$(PROJECT_INCLUDE)/sys/sysctl.h: sys/sysctl.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/sysctl.h

$(PROJECT_INCLUDE)/sys/syslog.h: sys/syslog.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/syslog.h

$(PROJECT_INCLUDE)/sys/systm.h: sys/systm.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/systm.h

$(PROJECT_INCLUDE)/sys/ttydefaults.h: sys/ttydefaults.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ttydefaults.h

$(PROJECT_INCLUDE)/sys/ucred.h: sys/ucred.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ucred.h

$(PROJECT_INCLUDE)/sys/uio.h: sys/uio.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/uio.h

$(PROJECT_INCLUDE)/sys/un.h: sys/un.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/un.h

$(PROJECT_INCLUDE)/vm:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/vm
PREINSTALL_FILES += $(PROJECT_INCLUDE)/vm

$(PROJECT_INCLUDE)/vm/vm.h: vm/vm.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/vm/vm.h

$(PROJECT_INCLUDE)/vm/vm_extern.h: vm/vm_extern.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/vm/vm_extern.h

$(PROJECT_INCLUDE)/vm/vm_kern.h: vm/vm_kern.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/vm/vm_kern.h

$(PROJECT_INCLUDE)/vm/vm_param.h: vm/vm_param.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/vm/vm_param.h
endif

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
