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

if LIBNETWORKING
noinst_LIBRARIES = libnetworking.a
libnetworking_a_SOURCES =
libnetworking_a_SOURCES += kern/kern_mib.c
libnetworking_a_SOURCES += kern/kern_subr.c
libnetworking_a_SOURCES += kern/kern_sysctl.c
libnetworking_a_SOURCES += kern/uipc_domain.c
libnetworking_a_SOURCES += kern/uipc_mbuf.c
libnetworking_a_SOURCES += kern/uipc_socket2.c
libnetworking_a_SOURCES += kern/uipc_socket.c
libnetworking_a_SOURCES += libc/base64.c
libnetworking_a_SOURCES += libc/gethostbydns.c
libnetworking_a_SOURCES += libc/gethostbyht.c
libnetworking_a_SOURCES += libc/gethostbynis.c
libnetworking_a_SOURCES += libc/gethostnamadr.c
libnetworking_a_SOURCES += libc/getifaddrs.c
libnetworking_a_SOURCES += libc/getnameinfo.c
libnetworking_a_SOURCES += libc/getnetbydns.c
libnetworking_a_SOURCES += libc/getnetbyht.c
libnetworking_a_SOURCES += libc/getnetbynis.c
libnetworking_a_SOURCES += libc/getnetnamadr.c
libnetworking_a_SOURCES += libc/getproto.c
libnetworking_a_SOURCES += libc/getprotoent.c
libnetworking_a_SOURCES += libc/getprotoname.c
libnetworking_a_SOURCES += libc/getservbyname.c
libnetworking_a_SOURCES += libc/getservbyport.c
libnetworking_a_SOURCES += libc/getservent.c
libnetworking_a_SOURCES += libc/herror.c
libnetworking_a_SOURCES += libc/if_indextoname.c
libnetworking_a_SOURCES += libc/if_nameindex.c
libnetworking_a_SOURCES += libc/inet_addr.c
libnetworking_a_SOURCES += libc/inet_lnaof.c
libnetworking_a_SOURCES += libc/inet_makeaddr.c
libnetworking_a_SOURCES += libc/inet_netof.c
libnetworking_a_SOURCES += libc/inet_network.c
libnetworking_a_SOURCES += libc/inet_ntoa.c
libnetworking_a_SOURCES += libc/inet_ntop.c
libnetworking_a_SOURCES += libc/inet_pton.c
libnetworking_a_SOURCES += libc/linkaddr.c
libnetworking_a_SOURCES += libc/map_v4v6.c
libnetworking_a_SOURCES += libc/nsap_addr.c
libnetworking_a_SOURCES += libc/ns_name.c
libnetworking_a_SOURCES += libc/ns_netint.c
libnetworking_a_SOURCES += libc/ns_parse.c
libnetworking_a_SOURCES += libc/ns_print.c
libnetworking_a_SOURCES += libc/ns_ttl.c
libnetworking_a_SOURCES += libc/rcmd.c
libnetworking_a_SOURCES += libc/recv.c
libnetworking_a_SOURCES += libc/res_comp.c
libnetworking_a_SOURCES += libc/res_data.c
libnetworking_a_SOURCES += libc/res_debug.c
libnetworking_a_SOURCES += libc/res_init.c
libnetworking_a_SOURCES += libc/res_mkquery.c
libnetworking_a_SOURCES += libc/res_mkupdate.c
libnetworking_a_SOURCES += libc/res_query.c
libnetworking_a_SOURCES += libc/res_send.c
libnetworking_a_SOURCES += libc/res_stubs.c
libnetworking_a_SOURCES += libc/res_update.c
libnetworking_a_SOURCES += libc/send.c
libnetworking_a_SOURCES += libc/strsep.c
libnetworking_a_SOURCES += lib/getprotoby.c
libnetworking_a_SOURCES += lib/rtems_bsdnet_ntp.c
libnetworking_a_SOURCES += lib/syslog.c
libnetworking_a_SOURCES += net/if.c
libnetworking_a_SOURCES += net/if_ethersubr.c
libnetworking_a_SOURCES += net/if_loop.c
libnetworking_a_SOURCES += net/if_ppp.c
libnetworking_a_SOURCES += netinet/if_ether.c
libnetworking_a_SOURCES += netinet/igmp.c
libnetworking_a_SOURCES += netinet/in.c
libnetworking_a_SOURCES += netinet/in_cksum.c
libnetworking_a_SOURCES += netinet/in_pcb.c
libnetworking_a_SOURCES += netinet/in_proto.c
libnetworking_a_SOURCES += netinet/in_rmx.c
libnetworking_a_SOURCES += netinet/ip_divert.c
libnetworking_a_SOURCES += netinet/ip_fw.c
libnetworking_a_SOURCES += netinet/ip_icmp.c
libnetworking_a_SOURCES += netinet/ip_input.c
libnetworking_a_SOURCES += netinet/ip_mroute.c
libnetworking_a_SOURCES += netinet/ip_output.c
libnetworking_a_SOURCES += netinet/raw_ip.c
libnetworking_a_SOURCES += netinet/tcp_debug.c
libnetworking_a_SOURCES += netinet/tcp_input.c
libnetworking_a_SOURCES += netinet/tcp_output.c
libnetworking_a_SOURCES += netinet/tcp_subr.c
libnetworking_a_SOURCES += netinet/tcp_timer.c
libnetworking_a_SOURCES += netinet/tcp_usrreq.c
libnetworking_a_SOURCES += netinet/udp_usrreq.c
libnetworking_a_SOURCES += net/ppp_tty.c
libnetworking_a_SOURCES += net/radix.c
libnetworking_a_SOURCES += net/raw_cb.c
libnetworking_a_SOURCES += net/raw_usrreq.c
libnetworking_a_SOURCES += net/route.c
libnetworking_a_SOURCES += net/rtsock.c
libnetworking_a_SOURCES += net/slcompress.c
libnetworking_a_SOURCES += nfs/bootp_subr.c
libnetworking_a_SOURCES += rtems/mkrootfs.c
libnetworking_a_SOURCES += rtems/rtems_bootp.c
libnetworking_a_SOURCES += rtems/rtems_bsdnet_malloc_starvation.c
libnetworking_a_SOURCES += rtems/rtems_dhcp.c
libnetworking_a_SOURCES += rtems/rtems_dhcp_failsafe.c
libnetworking_a_SOURCES += rtems/rtems_glue.c
libnetworking_a_SOURCES += rtems/rtems_malloc_mbuf.c
libnetworking_a_SOURCES += rtems/rtems_mii_ioctl.c
libnetworking_a_SOURCES += rtems/rtems_mii_ioctl_kern.c
libnetworking_a_SOURCES += rtems/rtems_select.c
libnetworking_a_SOURCES += rtems/rtems_showicmpstat.c
libnetworking_a_SOURCES += rtems/rtems_showifstat.c
libnetworking_a_SOURCES += rtems/rtems_showipstat.c
libnetworking_a_SOURCES += rtems/rtems_showmbuf.c
libnetworking_a_SOURCES += rtems/rtems_showroute.c
libnetworking_a_SOURCES += rtems/rtems_showtcpstat.c
libnetworking_a_SOURCES += rtems/rtems_showudpstat.c
libnetworking_a_SOURCES += rtems/rtems_socketpair.c
libnetworking_a_SOURCES += rtems/rtems_syscall_api.c
libnetworking_a_SOURCES += rtems/rtems_syscall.c
libnetworking_a_SOURCES += rtems/sghostname.c
endif

include $(top_srcdir)/automake/local.am
if LIBNETWORKING
include $(srcdir)/headers.am
endif
