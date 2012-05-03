include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

EXTRA_DIST =

if LIBRPC
include_rpcdir = $(includedir)/rpc

include_rpc_HEADERS = include/rpc/auth.h
include_rpc_HEADERS += include/rpc/auth_unix.h
include_rpc_HEADERS += include/rpc/clnt.h
include_rpc_HEADERS += include/rpc/clnt_stat.h
include_rpc_HEADERS += include/rpc/clnt_soc.h
include_rpc_HEADERS += include/rpc/svc.h
include_rpc_HEADERS += include/rpc/svc_auth.h
include_rpc_HEADERS += include/rpc/svc_soc.h
include_rpc_HEADERS += include/rpc/pmap_clnt.h
include_rpc_HEADERS += include/rpc/pmap_prot.h
include_rpc_HEADERS += include/rpc/pmap_rmt.h
include_rpc_HEADERS += include/rpc/rpc.h
include_rpc_HEADERS += include/rpc/types.h
include_rpc_HEADERS += include/rpc/xdr.h
include_rpc_HEADERS += include/rpc/rpc_com.h
include_rpc_HEADERS += include/rpc/rpc_msg.h
include_rpc_HEADERS += include/rpc/rpcent.h

man_MANS =

noinst_LIBRARIES = libxdr.a
libxdr_a_CPPFLAGS = $(AM_CPPFLAGS)

libxdr_a_SOURCES = src/xdr/xdr.c src/xdr/xdr_array.c src/xdr/xdr_float.c \
    src/xdr/xdr_mem.c src/xdr/xdr_rec.c src/xdr/xdr_reference.c \
    src/xdr/xdr_sizeof.c src/xdr/xdr_stdio.c
endif

libxdr_mans = src/xdr/xdr.3
EXTRA_DIST += $(libxdr_mans)

if LIBRPC
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
EXTRA_DIST += $(librpc_mans)

if LIBRPC
man_MANS += $(librpc_mans)
endif

EXTRA_DIST += src/rpc/DISCLAIMER src/rpc/README

EXTRA_DIST += README_RTEMS

## FIXME: Unused
EXTRA_DIST += include/rpcsvc/bootparam_prot.x include/rpcsvc/crypt.x \
    include/rpcsvc/key_prot.x include/rpcsvc/klm_prot.x \
    include/rpcsvc/mount.x include/rpcsvc/nfs_prot.x \
    include/rpcsvc/nis_cache.x include/rpcsvc/nis_callback.x \
    include/rpcsvc/nis_db.h include/rpcsvc/nislib.h \
    include/rpcsvc/nis_object.x include/rpcsvc/nis_tags.h \
    include/rpcsvc/nis.x include/rpcsvc/nlm_prot.x \
    include/rpcsvc/pmap_prot.x include/rpcsvc/rex.x include/rpcsvc/rnusers.x \
    include/rpcsvc/rquota.x include/rpcsvc/rstat.x include/rpcsvc/rwall.x \
    include/rpcsvc/sm_inter.x include/rpcsvc/spray.x include/rpcsvc/ypclnt.h \
    include/rpcsvc/yppasswd.x include/rpcsvc/yp_prot.h \
    include/rpcsvc/ypupdate_prot.x include/rpcsvc/yp.x \
    include/rpcsvc/ypxfrd.x

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
