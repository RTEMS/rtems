##
## $Id$
##

include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

SUBDIRS = PSD.doc

LIB = $(ARCH)/librpc.a

C_FILES = auth_none.c auth_unix.c authunix_prot.c bindresvport.c \
    clnt_generic.c clnt_perror.c clnt_raw.c clnt_simple.c clnt_tcp.c \
    clnt_udp.c get_myaddress.c getrpcent.c getrpcport.c netname.c netnamer.c \
    pmap_clnt.c pmap_getmaps.c pmap_getport.c pmap_prot.c pmap_prot2.c \
    pmap_rmt.c rpc_callmsg.c rpc_commondata.c rpc_dtablesize.c rpc_prot.c \
    rpcdname.c rtime.c svc.c svc_auth.c svc_auth_unix.c svc_raw.c svc_run.c \
    svc_simple.c svc_tcp.c svc_udp.c rtems_portmapper.c rtems_rpc.c
UNUSED_C_FILES = auth_des.c auth_time.c authdes_prot.c clnt_unix.c \
    crypt_client.c des_crypt.c des_soft.c getpublickey.c key_call.c \
    key_prot_xdr.c svc_auth_des.c svc_unix.c

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

#
# Add local stuff here using +=
#

AM_CPPFLAGS += '-D__P(x)=x' -D_RPC_read=read -D_RPC_write=write -D_RPC_close=close -D_RTEMS_RPC_INTERNAL_

$(LIB): $(OBJS)
	$(make-library)

noinst_MANS = bindresvport.3 getrpcent.3 publickey.3 rpc.3 rpc_secure.3 \
    rstat_svc.8 des_crypt.3 getrpcport.3 publickey.5 rpc.5 rstat.1 rtime.3

if HAS_NETWORKING
man_MANS = $(noinst_MANS)
all-local: $(ARCH) $(LIB)
endif

EXTRA_DIST = $(C_FILES) DISCLAIMER $(UNUSED_C_FILES) $(noinst_MANS)

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
