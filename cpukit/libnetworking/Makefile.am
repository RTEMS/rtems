##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/amcompile.am

AM_CPPFLAGS += -I$(top_builddir)

if HAS_NETWORKING

# Add -DFORWARD_PROTOCOL to enable UDP forwarding -- requires missing net/pf.h
libnetworking_CPPFLAGS = -D_COMPILING_BSD_KERNEL_ -DKERNEL -DINET -DNFS \
    -DDIAGNOSTIC -DBOOTP_COMPAT

EXTRA_LIBRARIES = libnetworking.a
libnetworking_a_SOURCES =
libnetworking_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) \
    $(libnetworking_CPPFLAGS)

EXTRA_LIBRARIES += libnetworking_g.a
libnetworking_g_a_SOURCES =
libnetworking_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) \
    $(libnetworking_CPPFLAGS)

TMP_LIBS = libnetworking$(LIB_VARIANT).a

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
libnetworking_a_SOURCES += $(kern_C_FILES)
libnetworking_g_a_SOURCES += $(kern_C_FILES)

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
libnetworking_a_SOURCES += $(net_C_FILES)
libnetworking_g_a_SOURCES += $(net_C_FILES)

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
libnetworking_a_SOURCES += $(netinet_C_FILES)
libnetworking_g_a_SOURCES += $(netinet_C_FILES)

## nfs

include_nfsdir = $(includedir)/nfs

include_nfs_HEADERS = nfs/krpc.h nfs/nfs.h nfs/nfsdiskless.h nfs/nfsproto.h \
    nfs/rpcv2.h nfs/xdr_subs.h

nfs_C_FILES = nfs/bootp_subr.c
libnetworking_a_SOURCES += $(nfs_C_FILES)
libnetworking_g_a_SOURCES += $(nfs_C_FILES)

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
libnetworking_a_SOURCES += $(rtems_C_FILES)
libnetworking_g_a_SOURCES += $(rtems_C_FILES)

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

## libc

libc_CPPFLAGS = -DNOPOLL -DNOSELECT

EXTRA_LIBRARIES += libc.a
libc_a_SOURCES =
libc_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) $(libc_CPPFLAGS)

EXTRA_LIBRARIES += libc_g.a
libc_g_a_SOURCES =
libc_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) $(libc_CPPFLAGS)

libc_C_FILES = libc/addr2ascii.c libc/ascii2addr.c libc/base64.c \
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
libc_a_SOURCES += $(libc_C_FILES)
libc_g_a_SOURCES += $(libc_C_FILES)

man_MANS = libc/addr2ascii.3 libc/byteorder.3 libc/ethers.3 \
    libc/gethostbyname.3 libc/getnetent.3 libc/getprotoent.3 \
    libc/getservent.3 libc/inet.3 libc/iso_addr.3 libc/linkaddr.3 libc/ns.3 \
    libc/rcmd.3 libc/resolver.3

TMP_LIBS += libc$(LIB_VARIANT).a

EXTRA_DIST = libc/addr2ascii.3 libc/addr2ascii.c libc/ascii2addr.c \
    libc/base64.c libc/byteorder.3 libc/ether_addr.c libc/ethers.3 \
    libc/gethostbydns.c libc/gethostbyht.c libc/gethostbyname.3 \
    libc/gethostbynis.c libc/gethostnamadr.c libc/gethostname.c \
    libc/getnetbydns.c libc/getnetbyht.c libc/getnetbynis.c libc/getnetent.3 \
    libc/getnetnamadr.c libc/getproto.c libc/getprotoent.3 \
    libc/getprotoent.c libc/getprotoname.c libc/getservbyname.c \
    libc/getservbyport.c libc/getservent.3 libc/getservent.c libc/herror.c \
    libc/inet.3 libc/inet_addr.c libc/inet_lnaof.c libc/inet_makeaddr.c \
    libc/inet_net_ntop.c libc/inet_net_pton.c libc/inet_neta.c \
    libc/inet_netof.c libc/inet_network.c libc/inet_ntoa.c libc/inet_ntop.c \
    libc/inet_pton.c libc/map_v4v6.c libc/linkaddr.3 libc/linkaddr.c \
    libc/ns.3 libc/ns_addr.c libc/ns_name.c libc/ns_netint.c libc/ns_ntoa.c \
    libc/ns_parse.c libc/ns_print.c libc/ns_ttl.c libc/nsap_addr.c \
    libc/rcmd.3 libc/rcmd.c libc/recv.c libc/res_comp.c libc/res_config.h \
    libc/res_data.c libc/res_debug.c libc/res_init.c libc/res_mkquery.c \
    libc/res_mkupdate.c libc/res_query.c libc/res_send.c libc/res_stubs.c \
    libc/res_update.c libc/resolver.3 libc/send.c libc/strsep.c

## lib

lib_CPPFLAGS = -DNOPOLL -DNOSELECT

EXTRA_LIBRARIES += lib.a
lib_a_SOURCES =
lib_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) $(lib_CPPFLAGS)

EXTRA_LIBRARIES += lib_g.a
lib_g_a_SOURCES =
lib_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) $(lib_CPPFLAGS)

lib_C_FILES = lib/getprotoby.c lib/rtems_bsdnet_ntp.c lib/ftpfs.c \
    lib/syslog.c lib/tftpDriver.c
lib_a_SOURCES += $(lib_C_FILES)
lib_g_a_SOURCES += $(lib_C_FILES)

TMP_LIBS += lib$(LIB_VARIANT).a

# ---
all-local: $(PREINSTALL_FILES) $(TMP_LIBS)
endif

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

if HAS_NETWORKING
$(PROJECT_INCLUDE)/bpfilter.h: bpfilter.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bpfilter.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bpfilter.h

$(PROJECT_INCLUDE)/loop.h: loop.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/loop.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/loop.h

$(PROJECT_INCLUDE)/netdb.h: netdb.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netdb.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netdb.h

$(PROJECT_INCLUDE)/opt_ipfw.h: opt_ipfw.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/opt_ipfw.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/opt_ipfw.h

$(PROJECT_INCLUDE)/opt_mrouting.h: opt_mrouting.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/opt_mrouting.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/opt_mrouting.h

$(PROJECT_INCLUDE)/opt_tcpdebug.h: opt_tcpdebug.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/opt_tcpdebug.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/opt_tcpdebug.h

$(PROJECT_INCLUDE)/resolv.h: resolv.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/resolv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/resolv.h

$(PROJECT_INCLUDE)/syslog.h: syslog.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/syslog.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/syslog.h

$(PROJECT_INCLUDE)/memory.h: memory.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/memory.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/memory.h

$(PROJECT_INCLUDE)/arpa/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/arpa
	@: > $(PROJECT_INCLUDE)/arpa/$(dirstamp)

$(PROJECT_INCLUDE)/arpa/ftp.h: arpa/ftp.h $(PROJECT_INCLUDE)/arpa/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/arpa/ftp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa/ftp.h

$(PROJECT_INCLUDE)/arpa/inet.h: arpa/inet.h $(PROJECT_INCLUDE)/arpa/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/arpa/inet.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa/inet.h

$(PROJECT_INCLUDE)/arpa/nameser.h: arpa/nameser.h $(PROJECT_INCLUDE)/arpa/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/arpa/nameser.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa/nameser.h

$(PROJECT_INCLUDE)/arpa/nameser_compat.h: arpa/nameser_compat.h $(PROJECT_INCLUDE)/arpa/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/arpa/nameser_compat.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa/nameser_compat.h

$(PROJECT_INCLUDE)/arpa/telnet.h: arpa/telnet.h $(PROJECT_INCLUDE)/arpa/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/arpa/telnet.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/arpa/telnet.h

$(PROJECT_INCLUDE)/machine/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/machine
	@: > $(PROJECT_INCLUDE)/machine/$(dirstamp)

$(PROJECT_INCLUDE)/machine/conf.h: machine/conf.h $(PROJECT_INCLUDE)/machine/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/machine/conf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine/conf.h

$(PROJECT_INCLUDE)/machine/cpu.h: machine/cpu.h $(PROJECT_INCLUDE)/machine/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/machine/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine/cpu.h

$(PROJECT_INCLUDE)/machine/cpufunc.h: machine/cpufunc.h $(PROJECT_INCLUDE)/machine/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/machine/cpufunc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine/cpufunc.h

$(PROJECT_INCLUDE)/machine/endian.h: machine/endian.h $(PROJECT_INCLUDE)/machine/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/machine/endian.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine/endian.h

$(PROJECT_INCLUDE)/machine/in_cksum.h: machine/in_cksum.h $(PROJECT_INCLUDE)/machine/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/machine/in_cksum.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine/in_cksum.h

$(PROJECT_INCLUDE)/machine/vmparam.h: machine/vmparam.h $(PROJECT_INCLUDE)/machine/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/machine/vmparam.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/machine/vmparam.h

$(PROJECT_INCLUDE)/net/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/net
	@: > $(PROJECT_INCLUDE)/net/$(dirstamp)

$(PROJECT_INCLUDE)/net/bpf.h: net/bpf.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/bpf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/bpf.h

$(PROJECT_INCLUDE)/net/ethernet.h: net/ethernet.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/ethernet.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/ethernet.h

$(PROJECT_INCLUDE)/net/if.h: net/if.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/if.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if.h

$(PROJECT_INCLUDE)/net/if_arp.h: net/if_arp.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/if_arp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if_arp.h

$(PROJECT_INCLUDE)/net/if_dl.h: net/if_dl.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/if_dl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if_dl.h

$(PROJECT_INCLUDE)/net/if_llc.h: net/if_llc.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/if_llc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if_llc.h

$(PROJECT_INCLUDE)/net/if_ppp.h: net/if_ppp.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/if_ppp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if_ppp.h

$(PROJECT_INCLUDE)/net/if_types.h: net/if_types.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/if_types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if_types.h

$(PROJECT_INCLUDE)/net/netisr.h: net/netisr.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/netisr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/netisr.h

$(PROJECT_INCLUDE)/net/ppp-comp.h: net/ppp-comp.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/ppp-comp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/ppp-comp.h

$(PROJECT_INCLUDE)/net/ppp_defs.h: net/ppp_defs.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/ppp_defs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/ppp_defs.h

$(PROJECT_INCLUDE)/net/radix.h: net/radix.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/radix.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/radix.h

$(PROJECT_INCLUDE)/net/raw_cb.h: net/raw_cb.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/raw_cb.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/raw_cb.h

$(PROJECT_INCLUDE)/net/route.h: net/route.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/route.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/route.h

$(PROJECT_INCLUDE)/net/if_pppvar.h: net/if_pppvar.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/if_pppvar.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/if_pppvar.h

$(PROJECT_INCLUDE)/net/pppcompress.h: net/pppcompress.h $(PROJECT_INCLUDE)/net/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/net/pppcompress.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/net/pppcompress.h

$(PROJECT_INCLUDE)/netinet/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/netinet
	@: > $(PROJECT_INCLUDE)/netinet/$(dirstamp)

$(PROJECT_INCLUDE)/netinet/icmp_var.h: netinet/icmp_var.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/icmp_var.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/icmp_var.h

$(PROJECT_INCLUDE)/netinet/if_ether.h: netinet/if_ether.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/if_ether.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/if_ether.h

$(PROJECT_INCLUDE)/netinet/igmp.h: netinet/igmp.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/igmp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/igmp.h

$(PROJECT_INCLUDE)/netinet/igmp_var.h: netinet/igmp_var.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/igmp_var.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/igmp_var.h

$(PROJECT_INCLUDE)/netinet/in.h: netinet/in.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/in.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/in.h

$(PROJECT_INCLUDE)/netinet/in_pcb.h: netinet/in_pcb.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/in_pcb.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/in_pcb.h

$(PROJECT_INCLUDE)/netinet/in_systm.h: netinet/in_systm.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/in_systm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/in_systm.h

$(PROJECT_INCLUDE)/netinet/in_var.h: netinet/in_var.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/in_var.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/in_var.h

$(PROJECT_INCLUDE)/netinet/ip.h: netinet/ip.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/ip.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/ip.h

$(PROJECT_INCLUDE)/netinet/ip_fw.h: netinet/ip_fw.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/ip_fw.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/ip_fw.h

$(PROJECT_INCLUDE)/netinet/ip_icmp.h: netinet/ip_icmp.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/ip_icmp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/ip_icmp.h

$(PROJECT_INCLUDE)/netinet/ip_mroute.h: netinet/ip_mroute.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/ip_mroute.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/ip_mroute.h

$(PROJECT_INCLUDE)/netinet/ip_var.h: netinet/ip_var.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/ip_var.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/ip_var.h

$(PROJECT_INCLUDE)/netinet/tcp.h: netinet/tcp.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/tcp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcp.h

$(PROJECT_INCLUDE)/netinet/tcp_debug.h: netinet/tcp_debug.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/tcp_debug.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcp_debug.h

$(PROJECT_INCLUDE)/netinet/tcp_fsm.h: netinet/tcp_fsm.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/tcp_fsm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcp_fsm.h

$(PROJECT_INCLUDE)/netinet/tcp_seq.h: netinet/tcp_seq.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/tcp_seq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcp_seq.h

$(PROJECT_INCLUDE)/netinet/tcp_timer.h: netinet/tcp_timer.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/tcp_timer.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcp_timer.h

$(PROJECT_INCLUDE)/netinet/tcp_var.h: netinet/tcp_var.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/tcp_var.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcp_var.h

$(PROJECT_INCLUDE)/netinet/tcpip.h: netinet/tcpip.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/tcpip.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/tcpip.h

$(PROJECT_INCLUDE)/netinet/udp.h: netinet/udp.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/udp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/udp.h

$(PROJECT_INCLUDE)/netinet/udp_var.h: netinet/udp_var.h $(PROJECT_INCLUDE)/netinet/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/netinet/udp_var.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/netinet/udp_var.h

$(PROJECT_INCLUDE)/nfs/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/nfs
	@: > $(PROJECT_INCLUDE)/nfs/$(dirstamp)

$(PROJECT_INCLUDE)/nfs/krpc.h: nfs/krpc.h $(PROJECT_INCLUDE)/nfs/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/nfs/krpc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs/krpc.h

$(PROJECT_INCLUDE)/nfs/nfs.h: nfs/nfs.h $(PROJECT_INCLUDE)/nfs/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/nfs/nfs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs/nfs.h

$(PROJECT_INCLUDE)/nfs/nfsdiskless.h: nfs/nfsdiskless.h $(PROJECT_INCLUDE)/nfs/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/nfs/nfsdiskless.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs/nfsdiskless.h

$(PROJECT_INCLUDE)/nfs/nfsproto.h: nfs/nfsproto.h $(PROJECT_INCLUDE)/nfs/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/nfs/nfsproto.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs/nfsproto.h

$(PROJECT_INCLUDE)/nfs/rpcv2.h: nfs/rpcv2.h $(PROJECT_INCLUDE)/nfs/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/nfs/rpcv2.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs/rpcv2.h

$(PROJECT_INCLUDE)/nfs/xdr_subs.h: nfs/xdr_subs.h $(PROJECT_INCLUDE)/nfs/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/nfs/xdr_subs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs/xdr_subs.h

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/rtems_bsdnet.h: rtems/rtems_bsdnet.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/rtems_bsdnet.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/rtems_bsdnet.h

$(PROJECT_INCLUDE)/rtems/rtems_bsdnet_internal.h: rtems/rtems_bsdnet_internal.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/rtems_bsdnet_internal.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/rtems_bsdnet_internal.h

$(PROJECT_INCLUDE)/rtems/tftp.h: rtems/tftp.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/tftp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/tftp.h

$(PROJECT_INCLUDE)/rtems/ftpfs.h: rtems/ftpfs.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/ftpfs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/ftpfs.h

$(PROJECT_INCLUDE)/rtems/mkrootfs.h: rtems/mkrootfs.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/mkrootfs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/mkrootfs.h

$(PROJECT_INCLUDE)/sys/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/sys
	@: > $(PROJECT_INCLUDE)/sys/$(dirstamp)

$(PROJECT_INCLUDE)/sys/buf.h: sys/buf.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/buf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/buf.h

$(PROJECT_INCLUDE)/sys/callout.h: sys/callout.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/callout.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/callout.h

$(PROJECT_INCLUDE)/sys/conf.h: sys/conf.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/conf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/conf.h

$(PROJECT_INCLUDE)/sys/domain.h: sys/domain.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/domain.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/domain.h

$(PROJECT_INCLUDE)/sys/kernel.h: sys/kernel.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/kernel.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/kernel.h

$(PROJECT_INCLUDE)/sys/libkern.h: sys/libkern.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/libkern.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/libkern.h

$(PROJECT_INCLUDE)/sys/linker_set.h: sys/linker_set.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/linker_set.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/linker_set.h

$(PROJECT_INCLUDE)/sys/malloc.h: sys/malloc.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/malloc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/malloc.h

$(PROJECT_INCLUDE)/sys/mbuf.h: sys/mbuf.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/mbuf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/mbuf.h

$(PROJECT_INCLUDE)/sys/mount.h: sys/mount.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/mount.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/mount.h

$(PROJECT_INCLUDE)/sys/proc.h: sys/proc.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/proc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/proc.h

$(PROJECT_INCLUDE)/sys/protosw.h: sys/protosw.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/protosw.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/protosw.h

$(PROJECT_INCLUDE)/sys/queue.h: sys/queue.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/queue.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/queue.h

$(PROJECT_INCLUDE)/sys/reboot.h: sys/reboot.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/reboot.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/reboot.h

$(PROJECT_INCLUDE)/sys/resourcevar.h: sys/resourcevar.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/resourcevar.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/resourcevar.h

$(PROJECT_INCLUDE)/sys/rtprio.h: sys/rtprio.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/rtprio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/rtprio.h

$(PROJECT_INCLUDE)/sys/select.h: sys/select.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/select.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/select.h

$(PROJECT_INCLUDE)/sys/signalvar.h: sys/signalvar.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/signalvar.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/signalvar.h

$(PROJECT_INCLUDE)/sys/socket.h: sys/socket.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/socket.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/socket.h

$(PROJECT_INCLUDE)/sys/socketvar.h: sys/socketvar.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/socketvar.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/socketvar.h

$(PROJECT_INCLUDE)/sys/sysctl.h: sys/sysctl.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/sysctl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/sysctl.h

$(PROJECT_INCLUDE)/sys/syslog.h: sys/syslog.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/syslog.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/syslog.h

$(PROJECT_INCLUDE)/sys/systm.h: sys/systm.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/systm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/systm.h

$(PROJECT_INCLUDE)/sys/ttydefaults.h: sys/ttydefaults.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/ttydefaults.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ttydefaults.h

$(PROJECT_INCLUDE)/sys/ucred.h: sys/ucred.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/ucred.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ucred.h

$(PROJECT_INCLUDE)/sys/uio.h: sys/uio.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/uio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/uio.h

$(PROJECT_INCLUDE)/sys/un.h: sys/un.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/un.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/un.h

$(PROJECT_INCLUDE)/vm/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/vm
	@: > $(PROJECT_INCLUDE)/vm/$(dirstamp)

$(PROJECT_INCLUDE)/vm/vm.h: vm/vm.h $(PROJECT_INCLUDE)/vm/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/vm/vm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/vm/vm.h

$(PROJECT_INCLUDE)/vm/vm_extern.h: vm/vm_extern.h $(PROJECT_INCLUDE)/vm/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/vm/vm_extern.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/vm/vm_extern.h

$(PROJECT_INCLUDE)/vm/vm_kern.h: vm/vm_kern.h $(PROJECT_INCLUDE)/vm/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/vm/vm_kern.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/vm/vm_kern.h

$(PROJECT_INCLUDE)/vm/vm_param.h: vm/vm_param.h $(PROJECT_INCLUDE)/vm/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/vm/vm_param.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/vm/vm_param.h
endif

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
