include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

project_lib_LIBRARIES =
TMPINSTALL_FILES =

project_lib_LIBRARIES += libftpfs.a
libftpfs_a_SOURCES = lib/ftpfs.c
$(PROJECT_LIB)/libftpfs.a: libftpfs.a
	$(INSTALL_DATA) $< $(PROJECT_LIB)/libftpfs.a
TMPINSTALL_FILES += $(PROJECT_LIB)/libftpfs.a

project_lib_LIBRARIES += libtftpfs.a
libtftpfs_a_SOURCES = lib/tftpDriver.c
$(PROJECT_LIB)/libtftpfs.a: libtftpfs.a
	$(INSTALL_DATA) $< $(PROJECT_LIB)/libtftpfs.a
TMPINSTALL_FILES += $(PROJECT_LIB)/libtftpfs.a

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
libnetworking_a_SOURCES = opt_atalk.h opt_bdg.h opt_compat.h opt_inet6.h \
    opt_inet.h opt_ipfw.h opt_ipx.h opt_mac.h opt_mrouting.h opt_netgraph.h \
    opt_tcpdebug.h opt_ipsec.h loop.h
libnetworking_a_SOURCES += opt_ppp.h
libnetworking_a_SOURCES += bpfilter.h

# arpa

## kern

libnetworking_a_SOURCES += kern/kern_mib.c kern/kern_subr.c \
    kern/kern_sysctl.c kern/uipc_domain.c kern/uipc_mbuf.c \
    kern/uipc_socket.c kern/uipc_socket2.c

## machine

libnetworking_a_SOURCES += machine/limits.h

## net

libnetworking_a_SOURCES += net/if.c net/if_ethersubr.c net/if_loop.c \
    net/radix.c net/route.c net/rtsock.c net/raw_cb.c net/raw_usrreq.c \
    net/if_ppp.c net/ppp_tty.c net/slcompress.c

## netinet

libnetworking_a_SOURCES += netinet/if_ether.c netinet/igmp.c netinet/in.c \
    netinet/in_cksum.c netinet/in_pcb.c netinet/in_proto.c netinet/in_rmx.c \
    netinet/ip_divert.c netinet/ip_fw.c netinet/ip_icmp.c netinet/ip_input.c \
    netinet/ip_mroute.c netinet/ip_output.c netinet/raw_ip.c \
    netinet/tcp_debug.c netinet/tcp_input.c netinet/tcp_output.c \
    netinet/tcp_subr.c netinet/tcp_timer.c netinet/tcp_usrreq.c \
    netinet/udp_usrreq.c netinet/in_cksum_arm.h netinet/in_cksum_i386.h \
    netinet/in_cksum_m68k.h netinet/in_cksum_powerpc.h

## netinet6


## nfs

libnetworking_a_SOURCES += nfs/bootp_subr.c

## nfsclient
## rtems

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

# vm

# dev
## libc

libc_CPPFLAGS = -DNOPOLL -DNOSELECT -D__BSD_VISIBLE -D_THREAD_SAFE

noinst_LIBRARIES += libc.a
libc_a_CPPFLAGS = $(AM_CPPFLAGS) $(libc_CPPFLAGS)

libc_a_SOURCES = libc/base64.c \
    libc/gethostbydns.c libc/gethostbyht.c libc/gethostbynis.c \
    libc/gethostnamadr.c libc/getnetbydns.c libc/getnetbyht.c \
    libc/getnetbynis.c libc/getnetnamadr.c libc/getnameinfo.c libc/getproto.c \
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

if LIBNETWORKING
man_MANS = $(libc_MANS)

## lib


lib_CPPFLAGS = -DNOPOLL -DNOSELECT

noinst_LIBRARIES += lib.a
lib_a_CPPFLAGS = $(AM_CPPFLAGS) $(lib_CPPFLAGS) -D__BSD_VISIBLE

lib_a_SOURCES = lib/getprotoby.c lib/rtems_bsdnet_ntp.c \
    lib/syslog.c
lib_a_SOURCES += rtems/rtems_syscall_api.c
endif


include $(top_srcdir)/automake/local.am
if LIBNETWORKING
include $(srcdir)/headers.am
endif
