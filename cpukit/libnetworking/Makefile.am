include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

AM_CPPFLAGS += -I$(srcdir)

EXTRA_DIST = README
EXTRA_DIST += rtems/bootp.h

# poll is not supported
UNUSED_FILES = poll.h
# Original FreeBSD file
UNUSED_FILES += sys/poll.h


if LIBNETWORKING
# Add -DFORWARD_PROTOCOL to enable UDP forwarding -- requires missing net/pf.h
libnetworking_CPPFLAGS = -DINET -DNFS \
    -DDIAGNOSTIC -DBOOTP_COMPAT -D_KERNEL -D__BSD_VISIBLE

noinst_LIBRARIES = libnetworking.a
libnetworking_a_CPPFLAGS = $(AM_CPPFLAGS) $(libnetworking_CPPFLAGS)

## poll.h is not in the list because RTEMS does not have poll()
## and installing poll.h confuses autoconf.
include_HEADERS = netdb.h
include_HEADERS += resolv.h
include_HEADERS += syslog.h

libnetworking_a_SOURCES = opt_atalk.h opt_bdg.h opt_compat.h opt_inet6.h \
    opt_inet.h opt_ipfw.h opt_ipx.h opt_mac.h opt_mrouting.h opt_netgraph.h \
    opt_tcpdebug.h opt_ipsec.h loop.h
libnetworking_a_SOURCES += opt_ppp.h
libnetworking_a_SOURCES += bpfilter.h

# arpa

include_arpadir = $(includedir)/arpa

include_arpa_HEADERS = arpa/ftp.h
include_arpa_HEADERS += arpa/inet.h
include_arpa_HEADERS += arpa/nameser.h
include_arpa_HEADERS += arpa/nameser_compat.h

## kern

libnetworking_a_SOURCES += kern/kern_mib.c kern/kern_subr.c \
    kern/kern_sysctl.c kern/uipc_domain.c kern/uipc_mbuf.c \
    kern/uipc_socket.c kern/uipc_socket2.c

## machine

include_machinedir = $(includedir)/machine

include_machine_HEADERS = machine/cpu.h
include_machine_HEADERS += machine/cpufunc.h
include_machine_HEADERS += machine/in_cksum.h
include_machine_HEADERS += machine/vmparam.h

libnetworking_a_SOURCES += machine/limits.h

## net

include_netdir = $(includedir)/net

include_net_HEADERS = net/bpf.h
include_net_HEADERS += net/ethernet.h
include_net_HEADERS += net/if.h
include_net_HEADERS += net/if_arp.h
include_net_HEADERS += net/if_dl.h
include_net_HEADERS += net/if_llc.h
include_net_HEADERS += net/if_ppp.h
include_net_HEADERS += net/if_types.h
include_net_HEADERS += net/if_var.h
include_net_HEADERS += net/netisr.h
include_net_HEADERS += net/ppp_defs.h
include_net_HEADERS += net/radix.h
include_net_HEADERS += net/raw_cb.h
include_net_HEADERS += net/route.h
include_net_HEADERS += net/if_pppvar.h
include_net_HEADERS += net/slcompress.h
include_net_HEADERS += net/if_media.h
include_net_HEADERS += net/ppp_comp.h

libnetworking_a_SOURCES += net/if.c net/if_ethersubr.c net/if_loop.c \
    net/radix.c net/route.c net/rtsock.c net/raw_cb.c net/raw_usrreq.c \
    net/if_ppp.c net/ppp_tty.c net/slcompress.c

## netinet

include_netinetdir = $(includedir)/netinet

include_netinet_HEADERS = netinet/icmp_var.h
include_netinet_HEADERS += netinet/if_ether.h
include_netinet_HEADERS += netinet/igmp.h
include_netinet_HEADERS += netinet/igmp_var.h
include_netinet_HEADERS += netinet/in.h
include_netinet_HEADERS += netinet/in_pcb.h
include_netinet_HEADERS += netinet/in_systm.h
include_netinet_HEADERS += netinet/in_var.h
include_netinet_HEADERS += netinet/ip.h
include_netinet_HEADERS += netinet/ip_fw.h
include_netinet_HEADERS += netinet/ip_icmp.h
include_netinet_HEADERS += netinet/ip_mroute.h
include_netinet_HEADERS += netinet/ip_var.h
include_netinet_HEADERS += netinet/tcp.h
include_netinet_HEADERS += netinet/tcp_debug.h
include_netinet_HEADERS += netinet/tcp_fsm.h
include_netinet_HEADERS += netinet/tcp_seq.h
include_netinet_HEADERS += netinet/tcp_timer.h
include_netinet_HEADERS += netinet/tcp_var.h
include_netinet_HEADERS += netinet/tcpip.h
include_netinet_HEADERS += netinet/udp.h
include_netinet_HEADERS += netinet/udp_var.h

libnetworking_a_SOURCES += netinet/if_ether.c netinet/igmp.c netinet/in.c \
    netinet/in_cksum.c netinet/in_pcb.c netinet/in_proto.c netinet/in_rmx.c \
    netinet/ip_divert.c netinet/ip_fw.c netinet/ip_icmp.c netinet/ip_input.c \
    netinet/ip_mroute.c netinet/ip_output.c netinet/raw_ip.c \
    netinet/tcp_debug.c netinet/tcp_input.c netinet/tcp_output.c \
    netinet/tcp_subr.c netinet/tcp_timer.c netinet/tcp_usrreq.c \
    netinet/udp_usrreq.c netinet/in_cksum_arm.h netinet/in_cksum_i386.h \
    netinet/in_cksum_m68k.h netinet/in_cksum_powerpc.h

## nfs

include_nfsdir = $(includedir)/nfs

include_nfs_HEADERS = nfs/nfsproto.h
include_nfs_HEADERS += nfs/rpcv2.h
include_nfs_HEADERS += nfs/xdr_subs.h

libnetworking_a_SOURCES += nfs/bootp_subr.c

## nfsclient
include_nfsclientdir = $(includedir)/nfsclient

include_nfsclient_HEADERS = nfsclient/nfsargs.h
include_nfsclient_HEADERS += nfsclient/nfsdiskless.h

## rtems

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = rtems/rtems_bsdnet.h
include_rtems_HEADERS += rtems/rtems_bsdnet_internal.h
include_rtems_HEADERS += rtems/dhcp.h
include_rtems_HEADERS += rtems/rtems_dhcp_failsafe.h
include_rtems_HEADERS += rtems/tftp.h
include_rtems_HEADERS += rtems/ftpfs.h
include_rtems_HEADERS += rtems/mkrootfs.h
include_rtems_HEADERS += rtems/rtems_mii_ioctl.h

libnetworking_a_SOURCES += rtems/sghostname.c \
    rtems/rtems_glue.c rtems/rtems_malloc_mbuf.c rtems/rtems_syscall.c \
    rtems/rtems_bootp.c rtems/rtems_dhcp.c rtems/rtems_dhcp_failsafe.c \
    rtems/rtems_showmbuf.c rtems/rtems_showroute.c rtems/rtems_showifstat.c \
    rtems/rtems_showipstat.c rtems/rtems_showicmpstat.c \
    rtems/rtems_showtcpstat.c rtems/rtems_showudpstat.c rtems/rtems_select.c \
    rtems/mkrootfs.c rtems/rtems_bsdnet_malloc_starvation.c \
    rtems/rtems_mii_ioctl.c rtems/rtems_mii_ioctl_kern.c \
    rtems/rtems_socketpair.c

## sys

include_sysdir = $(includedir)/sys

include_sys_HEADERS = sys/callout.h
include_sys_HEADERS += sys/conf.h
include_sys_HEADERS += sys/domain.h
include_sys_HEADERS += sys/kernel.h
include_sys_HEADERS += sys/libkern.h
include_sys_HEADERS += sys/linker_set.h
include_sys_HEADERS += sys/malloc.h
include_sys_HEADERS += sys/mbuf.h
include_sys_HEADERS += sys/mount.h
include_sys_HEADERS += sys/proc.h
include_sys_HEADERS += sys/protosw.h
include_sys_HEADERS += sys/reboot.h
include_sys_HEADERS += sys/resourcevar.h
include_sys_HEADERS += sys/select.h
include_sys_HEADERS += sys/signalvar.h
include_sys_HEADERS += sys/socket.h
include_sys_HEADERS += sys/socketvar.h
include_sys_HEADERS += sys/sysctl.h
include_sys_HEADERS += sys/syslog.h
include_sys_HEADERS += sys/systm.h
include_sys_HEADERS += sys/ttydefaults.h
include_sys_HEADERS += sys/ucred.h
include_sys_HEADERS += sys/un.h

# vm

include_vmdir = $(includedir)/vm

include_vm_HEADERS = vm/vm.h
include_vm_HEADERS += vm/vm_extern.h
include_vm_HEADERS += vm/vm_kern.h
include_vm_HEADERS += vm/vm_param.h

# dev
include_dev_miidir = $(includedir)/dev/mii
include_dev_mii_HEADERS = dev/mii/mii.h

## libc

libc_CPPFLAGS = -DNOPOLL -DNOSELECT -D__BSD_VISIBLE -D_THREAD_SAFE

noinst_LIBRARIES += libc.a
libc_a_CPPFLAGS = $(AM_CPPFLAGS) $(libc_CPPFLAGS)

include_HEADERS += ifaddrs.h

libc_a_SOURCES = libc/base64.c \
    libc/gethostbydns.c libc/gethostbyht.c libc/gethostbynis.c \
    libc/gethostnamadr.c libc/getnetbydns.c libc/getnetbyht.c \
    libc/getnetbynis.c libc/getnetnamadr.c libc/getproto.c \
    libc/getprotoent.c libc/getprotoname.c libc/getservbyname.c \
    libc/getservbyport.c libc/getservent.c libc/herror.c libc/inet_addr.c \
    libc/inet_lnaof.c libc/inet_makeaddr.c libc/inet_netof.c \
    libc/inet_network.c libc/inet_ntoa.c libc/inet_ntop.c libc/inet_pton.c \
    libc/linkaddr.c libc/map_v4v6.c libc/nsap_addr.c libc/ns_name.c \
    libc/ns_netint.c libc/ns_parse.c libc/ns_print.c libc/ns_ttl.c \
    libc/res_comp.c libc/res_data.c libc/res_debug.c libc/res_init.c \
    libc/res_mkquery.c libc/res_mkupdate.c libc/res_query.c libc/res_send.c \
    libc/res_stubs.c libc/res_update.c libc/strsep.c libc/rcmd.c
libc_a_SOURCES += libc/res_config.h
libc_a_SOURCES += libc/send.c
libc_a_SOURCES += libc/recv.c
libc_a_SOURCES += libc/getifaddrs.c
libc_a_SOURCES += libc/if_indextoname.c
libc_a_SOURCES += libc/if_nameindex.c
endif

UNUSED_FILES += libc/ether_addr.c libc/gethostname.c libc/inet_neta.c \
    libc/inet_net_ntop.c libc/inet_net_pton.c libc/ns_addr.c \
    libc/ns_ntoa.c

libc_mans = libc/byteorder.3 libc/ethers.3 \
    libc/gethostbyname.3 libc/getnetent.3 libc/getprotoent.3 \
    libc/getservent.3 libc/inet.3 libc/linkaddr.3 libc/ns.3 \
    libc/rcmd.3 libc/resolver.3
EXTRA_DIST += $(libc_mans)

if LIBNETWORKING
man_MANS = $(libc_MANS)

## lib

EXTRA_DIST += lib/README

lib_CPPFLAGS = -DNOPOLL -DNOSELECT

noinst_LIBRARIES += lib.a
lib_a_CPPFLAGS = $(AM_CPPFLAGS) $(lib_CPPFLAGS) -D__BSD_VISIBLE

lib_a_SOURCES = lib/getprotoby.c lib/rtems_bsdnet_ntp.c lib/ftpfs.c \
    lib/syslog.c lib/tftpDriver.c
endif

EXTRA_DIST += $(UNUSED_FILES)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
