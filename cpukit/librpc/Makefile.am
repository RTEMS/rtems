include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am


if LIBNETWORKING
man_MANS =

noinst_LIBRARIES = libxdr.a
libxdr_a_CPPFLAGS = $(AM_CPPFLAGS)

libxdr_a_SOURCES = src/xdr/xdr.c src/xdr/xdr_array.c src/xdr/xdr_float.c \
    src/xdr/xdr_mem.c src/xdr/xdr_rec.c src/xdr/xdr_reference.c \
    src/xdr/xdr_sizeof.c src/xdr/xdr_stdio.c
endif

libxdr_mans = src/xdr/xdr.3

if LIBNETWORKING
man_MANS += $(libxdr_mans)

noinst_LIBRARIES += librpc.a
librpc_a_CPPFLAGS = $(AM_CPPFLAGS) $(librpc_CPPFLAGS)

librpc_a_SOURCES = src/rpc/auth_none.c src/rpc/auth_unix.c \
    src/rpc/authunix_prot.c src/rpc/bindresvport.c src/rpc/clnt_generic.c \
    src/rpc/clnt_perror.c src/rpc/clnt_raw.c src/rpc/clnt_simple.c \
    src/rpc/clnt_tcp.c src/rpc/clnt_udp.c src/rpc/get_myaddress.c \
    src/rpc/getrpcent.c src/rpc/getrpcport.c src/rpc/netname.c \
    src/rpc/netnamer.c src/rpc/pmap_clnt.c src/rpc/pmap_getmaps.c \
    src/rpc/pmap_getport.c src/rpc/pmap_prot.c src/rpc/pmap_prot2.c \
    src/rpc/pmap_rmt.c src/rpc/rpc_callmsg.c src/rpc/rpc_commondata.c \
    src/rpc/rpc_dtablesize.c src/rpc/rpc_prot.c src/rpc/rpcdname.c \
    src/rpc/rtime.c src/rpc/svc.c src/rpc/svc_auth.c src/rpc/svc_auth_unix.c \
    src/rpc/svc_raw.c src/rpc/svc_run.c src/rpc/svc_simple.c \
    src/rpc/svc_tcp.c src/rpc/svc_udp.c src/rpc/rtems_portmapper.c \
    src/rpc/rtems_rpc.c

librpc_CPPFLAGS = -D_RPC_read=read -D_RPC_write=write -D_RPC_close=close \
    -D_RTEMS_RPC_INTERNAL_
endif

librpc_mans = src/rpc/bindresvport.3 src/rpc/getrpcent.3 src/rpc/publickey.3 \
    src/rpc/rpc.3 src/rpc/rpc_secure.3 src/rpc/rstat_svc.8 \
    src/rpc/des_crypt.3 src/rpc/getrpcport.3 src/rpc/publickey.5 \
    src/rpc/rpc.5 src/rpc/rstat.1 src/rpc/rtime.3

if LIBNETWORKING
man_MANS += $(librpc_mans)
endif



## FIXME: Unused

include $(top_srcdir)/automake/local.am
