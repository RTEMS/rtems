##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

AM_CPPFLAGS += -I$(srcdir)

EXTRA_DIST = README

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
include_HEADERS += resolv.h syslog.h

libnetworking_a_SOURCES = opt_atalk.h opt_bdg.h opt_compat.h opt_inet6.h \
    opt_inet.h opt_ipfw.h opt_ipx.h opt_mac.h opt_mrouting.h opt_netgraph.h \
    opt_tcpdebug.h opt_ipsec.h loop.h
libnetworking_a_SOURCES += opt_ppp.h
libnetworking_a_SOURCES += bpfilter.h

# arpa

include_arpadir = $(includedir)/arpa

include_arpa_HEADERS = arpa/ftp.h arpa/inet.h arpa/nameser.h \
    arpa/nameser_compat.h

## kern

libnetworking_a_SOURCES += kern/kern_mib.c kern/kern_subr.c \
    kern/kern_sysctl.c kern/uipc_domain.c kern/uipc_mbuf.c \
    kern/uipc_socket.c kern/uipc_socket2.c

## machine

include_machinedir = $(includedir)/machine

include_machine_HEADERS = machine/cpu.h machine/cpufunc.h \
    machine/in_cksum.h machine/vmparam.h

libnetworking_a_SOURCES += machine/limits.h

## net

include_netdir = $(includedir)/net

include_net_HEADERS = net/bpf.h net/ethernet.h net/if.h net/if_arp.h \
    net/if_dl.h net/if_llc.h net/if_ppp.h net/if_types.h net/if_var.h \
    net/netisr.h net/ppp_defs.h net/radix.h net/raw_cb.h \
    net/route.h net/if_pppvar.h net/slcompress.h
include_net_HEADERS += net/if_media.h
include_net_HEADERS += net/ppp_comp.h

libnetworking_a_SOURCES += net/if.c net/if_ethersubr.c net/if_loop.c \
    net/radix.c net/route.c net/rtsock.c net/raw_cb.c net/raw_usrreq.c \
    net/if_ppp.c net/ppp_tty.c net/slcompress.c

## netinet

include_netinetdir = $(includedir)/netinet

include_netinet_HEADERS = netinet/icmp_var.h netinet/if_ether.h \
    netinet/igmp.h netinet/igmp_var.h netinet/in.h netinet/in_pcb.h \
    netinet/in_systm.h netinet/in_var.h netinet/ip.h netinet/ip_fw.h \
    netinet/ip_icmp.h netinet/ip_mroute.h netinet/ip_var.h netinet/tcp.h \
    netinet/tcp_debug.h netinet/tcp_fsm.h netinet/tcp_seq.h \
    netinet/tcp_timer.h netinet/tcp_var.h netinet/tcpip.h netinet/udp.h \
    netinet/udp_var.h

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

include_nfsclient_HEADERS = nfsclient/nfsargs.h nfsclient/nfsdiskless.h

## rtems

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = rtems/rtems_bsdnet.h rtems/rtems_bsdnet_internal.h \
    rtems/dhcp.h rtems/rtems_dhcp_failsafe.h rtems/tftp.h rtems/ftpfs.h \
    rtems/mkrootfs.h
include_rtems_HEADERS += rtems/rtems_mii_ioctl.h

libnetworking_a_SOURCES += rtems/sghostname.c rtems/issetugid.c \
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

include_sys_HEADERS = sys/callout.h sys/conf.h sys/domain.h \
    sys/kernel.h sys/libkern.h sys/linker_set.h sys/malloc.h sys/mbuf.h \
    sys/mount.h sys/proc.h sys/protosw.h sys/reboot.h \
    sys/resourcevar.h sys/select.h sys/signalvar.h sys/socket.h \
    sys/socketvar.h sys/sysctl.h sys/syslog.h sys/systm.h sys/ttydefaults.h \
    sys/ucred.h sys/un.h

# vm

include_vmdir = $(includedir)/vm

include_vm_HEADERS = vm/vm.h vm/vm_extern.h vm/vm_kern.h vm/vm_param.h

# dev
include_dev_miidir = $(includedir)/dev/mii
include_dev_mii_HEADERS = dev/mii/mii.h

## libc

libc_CPPFLAGS = -DNOPOLL -DNOSELECT -D__BSD_VISIBLE -D_THREAD_SAFE

noinst_LIBRARIES += libc.a
libc_a_CPPFLAGS = $(AM_CPPFLAGS) $(libc_CPPFLAGS)

include_HEADERS += ifaddrs.h

libc_a_SOURCES = libc/addr2ascii.c libc/ascii2addr.c libc/base64.c \
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

libc_mans = libc/addr2ascii.3 libc/byteorder.3 libc/ethers.3 \
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
