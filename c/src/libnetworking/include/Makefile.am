##
## $Id$
##
##
## FIXME: This Makefile.am is pretty ugly and deserves to be reworked :(
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = ../bpfilter.h ../loop.h ../netdb.h ../opt_ipfw.h ../opt_mrouting.h \
    ../opt_tcpdebug.h ../poll.h ../resolv.h ../syslog.h ../memory.h

SYS_H_FILES = ../sys/buf.h ../sys/callout.h ../sys/cdefs.h ../sys/conf.h \
    ../sys/domain.h ../sys/filio.h ../sys/ioccom.h ../sys/ioctl.h \
    ../sys/kernel.h ../sys/libkern.h ../sys/malloc.h ../sys/mbuf.h \
    ../sys/mount.h ../sys/param.h ../sys/proc.h ../sys/protosw.h \
    ../sys/queue.h ../sys/reboot.h ../sys/resourcevar.h ../sys/rtprio.h \
    ../sys/select.h ../sys/signalvar.h ../sys/socket.h ../sys/socketvar.h \
    ../sys/sockio.h ../sys/sysctl.h ../sys/syslimits.h ../sys/syslog.h \
    ../sys/systm.h ../sys/ttycom.h ../sys/ttydefaults.h ../sys/ucred.h \
    ../sys/uio.h ../sys/un.h

RTEMS_H_FILES = ../rtems/rtems_bsdnet.h ../rtems/rtems_bsdnet_internal.h \
    ../rtems/tftp.h

MACHINE_H_FILES = ../machine/conf.h ../machine/cpu.h ../machine/cpufunc.h \
    ../machine/endian.h ../machine/in_cksum.h ../machine/limits.h \
    ../machine/param.h ../machine/types.h ../machine/vmparam.h

VM_H_FILES = ../vm/vm.h ../vm/vm_extern.h ../vm/vm_kern.h ../vm/vm_param.h

NET_H_FILES = ../net/bpf.h ../net/ethernet.h ../net/if.h ../net/if_arp.h \
    ../net/if_dl.h ../net/if_llc.h ../net/if_ppp.h ../net/if_types.h \
    ../net/netisr.h ../net/ppp-comp.h ../net/ppp_defs.h ../net/radix.h \
    ../net/raw_cb.h ../net/route.h

NETINET_H_FILES = ../netinet/icmp_var.h ../netinet/if_ether.h \
    ../netinet/igmp.h ../netinet/igmp_var.h ../netinet/in.h \
    ../netinet/in_pcb.h ../netinet/in_systm.h ../netinet/in_var.h \
    ../netinet/ip.h ../netinet/ip_fw.h ../netinet/ip_icmp.h \
    ../netinet/ip_mroute.h ../netinet/ip_var.h ../netinet/tcp.h \
    ../netinet/tcp_debug.h ../netinet/tcp_fsm.h ../netinet/tcp_seq.h \
    ../netinet/tcp_timer.h ../netinet/tcp_var.h ../netinet/tcpip.h \
    ../netinet/udp.h ../netinet/udp_var.h

ARPA_H_FILES = ../arpa/ftp.h ../arpa/inet.h ../arpa/nameser.h \
    ../arpa/nameser_compat.h ../arpa/telnet.h

NFS_H_FILES = ../nfs/krpc.h ../nfs/nfs.h ../nfs/nfsdiskless.h \
    ../nfs/nfsproto.h ../nfs/rpcv2.h ../nfs/xdr_subs.h

RTEMS_SERVERS_H_FILES = ../rtems_servers/ftpd.h

RTEMS_WEBSERVER_H_FILES = ../rtems_webserver/rtems_webserver.h

# We only make the install point for the networking header files if needed.

INSTALLDIRS = $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/sys \
    $(PROJECT_INCLUDE)/machine $(PROJECT_INCLUDE)/rtems \
    $(PROJECT_INCLUDE)/vm $(PROJECT_INCLUDE)/net $(PROJECT_INCLUDE)/netinet \
    $(PROJECT_INCLUDE)/arpa $(PROJECT_INCLUDE)/nfs

$(INSTALLDIRS):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bpfilter.h: ../bpfilter.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/loop.h: ../loop.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netdb.h: ../netdb.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/opt_ipfw.h: ../opt_ipfw.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/opt_mrouting.h: ../opt_mrouting.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/opt_tcpdebug.h: ../opt_tcpdebug.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/poll.h: ../poll.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/resolv.h: ../resolv.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/syslog.h: ../syslog.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/memory.h: ../memory.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/buf.h: ../sys/buf.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/callout.h: ../sys/callout.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/cdefs.h: ../sys/cdefs.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/conf.h: ../sys/conf.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/domain.h: ../sys/domain.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/filio.h: ../sys/filio.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/ioccom.h: ../sys/ioccom.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/ioctl.h: ../sys/ioctl.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/kernel.h: ../sys/kernel.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/libkern.h: ../sys/libkern.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/malloc.h: ../sys/malloc.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/mbuf.h: ../sys/mbuf.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/mount.h: ../sys/mount.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/param.h: ../sys/param.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/proc.h: ../sys/proc.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/protosw.h: ../sys/protosw.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/queue.h: ../sys/queue.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/reboot.h: ../sys/reboot.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/resourcevar.h: ../sys/resourcevar.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/rtprio.h: ../sys/rtprio.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/select.h: ../sys/select.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/signalvar.h: ../sys/signalvar.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/socket.h: ../sys/socket.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/socketvar.h: ../sys/socketvar.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/sockio.h: ../sys/sockio.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/sysctl.h: ../sys/sysctl.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/syslimits.h: ../sys/syslimits.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/syslog.h: ../sys/syslog.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/systm.h: ../sys/systm.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/ttycom.h: ../sys/ttycom.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/ttydefaults.h: ../sys/ttydefaults.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/ucred.h: ../sys/ucred.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/uio.h: ../sys/uio.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sys/un.h: ../sys/un.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems/rtems_bsdnet.h: ../rtems/rtems_bsdnet.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems/rtems_bsdnet_internal.h: \
    ../rtems/rtems_bsdnet_internal.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems/tftp.h: ../rtems/tftp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/machine/conf.h: ../machine/conf.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/machine/cpu.h: ../machine/cpu.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/machine/cpufunc.h: ../machine/cpufunc.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/machine/endian.h: ../machine/endian.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/machine/in_cksum.h: ../machine/in_cksum.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/machine/limits.h: ../machine/limits.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/machine/param.h: ../machine/param.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/machine/types.h: ../machine/types.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/machine/vmparam.h: ../machine/vmparam.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/vm/vm.h: ../vm/vm.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/vm/vm_extern.h: ../vm/vm_extern.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/vm/vm_kern.h: ../vm/vm_kern.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/vm/vm_param.h: ../vm/vm_param.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/bpf.h: ../net/bpf.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/ethernet.h: ../net/ethernet.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/if.h: ../net/if.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/if_arp.h: ../net/if_arp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/if_dl.h: ../net/if_dl.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/if_llc.h: ../net/if_llc.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/if_ppp.h: ../net/if_ppp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/if_types.h: ../net/if_types.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/netisr.h: ../net/netisr.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/ppp-comp.h: ../net/ppp-comp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/ppp_defs.h: ../net/ppp_defs.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/radix.h: ../net/radix.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/raw_cb.h: ../net/raw_cb.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/net/route.h: ../net/route.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/icmp_var.h: ../netinet/icmp_var.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/if_ether.h: ../netinet/if_ether.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/igmp.h: ../netinet/igmp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/igmp_var.h: ../netinet/igmp_var.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/in.h: ../netinet/in.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/in_pcb.h: ../netinet/in_pcb.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/in_systm.h: ../netinet/in_systm.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/in_var.h: ../netinet/in_var.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/ip.h: ../netinet/ip.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/ip_fw.h: ../netinet/ip_fw.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/ip_icmp.h: ../netinet/ip_icmp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/ip_mroute.h: ../netinet/ip_mroute.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/ip_var.h: ../netinet/ip_var.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/tcp.h: ../netinet/tcp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/tcp_debug.h: ../netinet/tcp_debug.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/tcp_fsm.h: ../netinet/tcp_fsm.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/tcp_seq.h: ../netinet/tcp_seq.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/tcp_timer.h: ../netinet/tcp_timer.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/tcp_var.h: ../netinet/tcp_var.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/tcpip.h: ../netinet/tcpip.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/udp.h: ../netinet/udp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/netinet/udp_var.h: ../netinet/udp_var.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/arpa/ftp.h: ../arpa/ftp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/arpa/inet.h: ../arpa/inet.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/arpa/nameser.h: ../arpa/nameser.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/arpa/nameser_compat.h: ../arpa/nameser_compat.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/arpa/telnet.h: ../arpa/telnet.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/nfs/krpc.h: ../nfs/krpc.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/nfs/nfs.h: ../nfs/nfs.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/nfs/nfsdiskless.h: ../nfs/nfsdiskless.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/nfs/nfsproto.h: ../nfs/nfsproto.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/nfs/rpcv2.h: ../nfs/rpcv2.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/nfs/xdr_subs.h: ../nfs/xdr_subs.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/ftpd.h: ../rtems_servers/ftpd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems_webserver.h: ../rtems_webserver/rtems_webserver.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bpfilter.h \
    $(PROJECT_INCLUDE)/loop.h $(PROJECT_INCLUDE)/netdb.h \
    $(PROJECT_INCLUDE)/opt_ipfw.h $(PROJECT_INCLUDE)/opt_mrouting.h \
    $(PROJECT_INCLUDE)/opt_tcpdebug.h $(PROJECT_INCLUDE)/poll.h \
    $(PROJECT_INCLUDE)/resolv.h $(PROJECT_INCLUDE)/syslog.h \
    $(PROJECT_INCLUDE)/memory.h \
    $(PROJECT_INCLUDE)/sys $(PROJECT_INCLUDE)/sys/buf.h \
    $(PROJECT_INCLUDE)/sys/callout.h $(PROJECT_INCLUDE)/sys/cdefs.h \
    $(PROJECT_INCLUDE)/sys/conf.h $(PROJECT_INCLUDE)/sys/domain.h \
    $(PROJECT_INCLUDE)/sys/filio.h $(PROJECT_INCLUDE)/sys/ioccom.h \
    $(PROJECT_INCLUDE)/sys/ioctl.h $(PROJECT_INCLUDE)/sys/kernel.h \
    $(PROJECT_INCLUDE)/sys/libkern.h $(PROJECT_INCLUDE)/sys/malloc.h \
    $(PROJECT_INCLUDE)/sys/mbuf.h $(PROJECT_INCLUDE)/sys/mount.h \
    $(PROJECT_INCLUDE)/sys/param.h $(PROJECT_INCLUDE)/sys/proc.h \
    $(PROJECT_INCLUDE)/sys/protosw.h $(PROJECT_INCLUDE)/sys/queue.h \
    $(PROJECT_INCLUDE)/sys/reboot.h $(PROJECT_INCLUDE)/sys/resourcevar.h \
    $(PROJECT_INCLUDE)/sys/rtprio.h $(PROJECT_INCLUDE)/sys/select.h \
    $(PROJECT_INCLUDE)/sys/signalvar.h $(PROJECT_INCLUDE)/sys/socket.h \
    $(PROJECT_INCLUDE)/sys/socketvar.h $(PROJECT_INCLUDE)/sys/sockio.h \
    $(PROJECT_INCLUDE)/sys/sysctl.h $(PROJECT_INCLUDE)/sys/syslimits.h \
    $(PROJECT_INCLUDE)/sys/syslog.h $(PROJECT_INCLUDE)/sys/systm.h \
    $(PROJECT_INCLUDE)/sys/ttycom.h $(PROJECT_INCLUDE)/sys/ttydefaults.h \
    $(PROJECT_INCLUDE)/sys/ucred.h $(PROJECT_INCLUDE)/sys/uio.h \
    $(PROJECT_INCLUDE)/sys/un.h \
    $(PROJECT_INCLUDE)/rtems $(PROJECT_INCLUDE)/rtems/rtems_bsdnet.h \
    $(PROJECT_INCLUDE)/rtems/rtems_bsdnet_internal.h \
    $(PROJECT_INCLUDE)/rtems/tftp.h $(PROJECT_INCLUDE)/machine \
    $(PROJECT_INCLUDE)/machine/conf.h $(PROJECT_INCLUDE)/machine/cpu.h \
    $(PROJECT_INCLUDE)/machine/cpufunc.h $(PROJECT_INCLUDE)/machine/endian.h \
    $(PROJECT_INCLUDE)/machine/in_cksum.h \
    $(PROJECT_INCLUDE)/machine/limits.h $(PROJECT_INCLUDE)/machine/param.h \
    $(PROJECT_INCLUDE)/machine/types.h $(PROJECT_INCLUDE)/machine/vmparam.h \
    $(PROJECT_INCLUDE)/vm $(PROJECT_INCLUDE)/vm/vm.h \
    $(PROJECT_INCLUDE)/vm/vm_extern.h $(PROJECT_INCLUDE)/vm/vm_kern.h \
    $(PROJECT_INCLUDE)/vm/vm_param.h $(PROJECT_INCLUDE)/net \
    $(PROJECT_INCLUDE)/net/bpf.h $(PROJECT_INCLUDE)/net/ethernet.h \
    $(PROJECT_INCLUDE)/net/if.h $(PROJECT_INCLUDE)/net/if_arp.h \
    $(PROJECT_INCLUDE)/net/if_dl.h $(PROJECT_INCLUDE)/net/if_llc.h \
    $(PROJECT_INCLUDE)/net/if_ppp.h $(PROJECT_INCLUDE)/net/if_types.h \
    $(PROJECT_INCLUDE)/net/netisr.h $(PROJECT_INCLUDE)/net/ppp-comp.h \
    $(PROJECT_INCLUDE)/net/ppp_defs.h $(PROJECT_INCLUDE)/net/radix.h \
    $(PROJECT_INCLUDE)/net/raw_cb.h $(PROJECT_INCLUDE)/net/route.h \
    $(PROJECT_INCLUDE)/netinet $(PROJECT_INCLUDE)/netinet/icmp_var.h \
    $(PROJECT_INCLUDE)/netinet/if_ether.h $(PROJECT_INCLUDE)/netinet/igmp.h \
    $(PROJECT_INCLUDE)/netinet/igmp_var.h $(PROJECT_INCLUDE)/netinet/in.h \
    $(PROJECT_INCLUDE)/netinet/in_pcb.h \
    $(PROJECT_INCLUDE)/netinet/in_systm.h \
    $(PROJECT_INCLUDE)/netinet/in_var.h $(PROJECT_INCLUDE)/netinet/ip.h \
    $(PROJECT_INCLUDE)/netinet/ip_fw.h $(PROJECT_INCLUDE)/netinet/ip_icmp.h \
    $(PROJECT_INCLUDE)/netinet/ip_mroute.h \
    $(PROJECT_INCLUDE)/netinet/ip_var.h $(PROJECT_INCLUDE)/netinet/tcp.h \
    $(PROJECT_INCLUDE)/netinet/tcp_debug.h \
    $(PROJECT_INCLUDE)/netinet/tcp_fsm.h \
    $(PROJECT_INCLUDE)/netinet/tcp_seq.h \
    $(PROJECT_INCLUDE)/netinet/tcp_timer.h \
    $(PROJECT_INCLUDE)/netinet/tcp_var.h $(PROJECT_INCLUDE)/netinet/tcpip.h \
    $(PROJECT_INCLUDE)/netinet/udp.h $(PROJECT_INCLUDE)/netinet/udp_var.h \
    $(PROJECT_INCLUDE)/arpa $(PROJECT_INCLUDE)/arpa/ftp.h \
    $(PROJECT_INCLUDE)/arpa/inet.h $(PROJECT_INCLUDE)/arpa/nameser.h \
    $(PROJECT_INCLUDE)/arpa/nameser_compat.h \
    $(PROJECT_INCLUDE)/arpa/telnet.h $(PROJECT_INCLUDE)/nfs \
    $(PROJECT_INCLUDE)/nfs/krpc.h $(PROJECT_INCLUDE)/nfs/nfs.h \
    $(PROJECT_INCLUDE)/nfs/nfsdiskless.h $(PROJECT_INCLUDE)/nfs/nfsproto.h \
    $(PROJECT_INCLUDE)/nfs/rpcv2.h $(PROJECT_INCLUDE)/nfs/xdr_subs.h \
    $(PROJECT_INCLUDE)/ftpd.h $(PROJECT_INCLUDE)/rtems_webserver.h

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
