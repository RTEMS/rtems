##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = . kern lib libc net netinet nfs rtems
SUBDIRS += wrapup

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

## nfs

include_nfsdir = $(includedir)/nfs

include_nfs_HEADERS = nfs/krpc.h nfs/nfs.h nfs/nfsdiskless.h nfs/nfsproto.h \
    nfs/rpcv2.h nfs/xdr_subs.h

$(PROJECT_INCLUDE)/nfs:
	@$(mkinstalldirs) $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/nfs \
    $(include_nfs_HEADERS:%=$(PROJECT_INCLUDE)/%)

## sys

include_sysdir = $(includedir)/sys

include_sys_HEADERS = sys/buf.h sys/callout.h sys/conf.h sys/domain.h \
    sys/kernel.h sys/libkern.h sys/malloc.h sys/mbuf.h sys/mount.h \
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

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
