##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

EXTRA_DIST =
CLEANFILES =

if LIBRPC
include_rpcdir = $(includedir)/rpc
include_rpc_HEADERS = include/rpc/auth.h include/rpc/auth_des.h \
    include/rpc/auth_unix.h include/rpc/clnt.h include/rpc/des.h \
    include/rpc/des_crypt.h include/rpc/pmap_clnt.h include/rpc/pmap_prot.h \
    include/rpc/pmap_rmt.h include/rpc/rpc.h include/rpc/rpc_com.h \
    include/rpc/rpc_msg.h include/rpc/svc.h include/rpc/svc_auth.h \
    include/rpc/types.h include/rpc/xdr.h

man_MANS =

EXTRA_LIBRARIES = libxdr.a
CLEANFILES += libxdr.a
libxdr_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libxdr_g.a
CLEANFILES += libxdr_g.a
libxdr_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

libxdr_a_SOURCES = src/xdr/xdr.c src/xdr/xdr_array.c src/xdr/xdr_float.c \
    src/xdr/xdr_mem.c src/xdr/xdr_rec.c src/xdr/xdr_reference.c \
    src/xdr/xdr_sizeof.c src/xdr/xdr_stdio.c
libxdr_g_a_SOURCES = $(libxdr_a_SOURCES)
endif

libxdr_mans = src/xdr/xdr.3
EXTRA_DIST += $(libxdr_mans)

if LIBRPC
man_MANS += $(libxdr_mans)

all-local: libxdr$(LIB_VARIANT).a

EXTRA_LIBRARIES += librpc.a
CLEANFILES += librpc.a
librpc_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) $(librpc_CPPFLAGS)

EXTRA_LIBRARIES += librpc_g.a
CLEANFILES += librpc_g.a
librpc_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) $(librpc_CPPFLAGS)

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
# librpc_UNUSED_C_FILES
EXTRA_DIST += src/rpc/auth_des.c src/rpc/auth_time.c src/rpc/authdes_prot.c \
    src/rpc/clnt_unix.c src/rpc/crypt_client.c src/rpc/des_crypt.c \
    src/rpc/des_soft.c src/rpc/getpublickey.c src/rpc/key_call.c \
    src/rpc/key_prot_xdr.c src/rpc/svc_auth_des.c src/rpc/svc_unix.c
librpc_g_a_SOURCES = $(librpc_a_SOURCES)

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

all-local: librpc$(LIB_VARIANT).a
endif

EXTRA_DIST += src/rpc/DISCLAIMER src/rpc/README

EXTRA_DIST += src/rpc/PSD.doc/nfs.rfc.ms src/rpc/PSD.doc/rpc.prog.ms \
    src/rpc/PSD.doc/rpc.rfc.ms src/rpc/PSD.doc/rpcgen.ms \
    src/rpc/PSD.doc/xdr.nts.ms src/rpc/PSD.doc/xdr.rfc.ms
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

PREINSTALL_DIRS =
PREINSTALL_FILES =

if LIBRPC
$(PROJECT_INCLUDE)/rpc/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rpc
	@: > $(PROJECT_INCLUDE)/rpc/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rpc/$(dirstamp)

$(PROJECT_INCLUDE)/rpc/auth.h: include/rpc/auth.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/auth.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/auth.h

$(PROJECT_INCLUDE)/rpc/auth_des.h: include/rpc/auth_des.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/auth_des.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/auth_des.h

$(PROJECT_INCLUDE)/rpc/auth_unix.h: include/rpc/auth_unix.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/auth_unix.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/auth_unix.h

$(PROJECT_INCLUDE)/rpc/clnt.h: include/rpc/clnt.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/clnt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/clnt.h

$(PROJECT_INCLUDE)/rpc/des.h: include/rpc/des.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/des.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/des.h

$(PROJECT_INCLUDE)/rpc/des_crypt.h: include/rpc/des_crypt.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/des_crypt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/des_crypt.h

$(PROJECT_INCLUDE)/rpc/pmap_clnt.h: include/rpc/pmap_clnt.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/pmap_clnt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/pmap_clnt.h

$(PROJECT_INCLUDE)/rpc/pmap_prot.h: include/rpc/pmap_prot.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/pmap_prot.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/pmap_prot.h

$(PROJECT_INCLUDE)/rpc/pmap_rmt.h: include/rpc/pmap_rmt.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/pmap_rmt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/pmap_rmt.h

$(PROJECT_INCLUDE)/rpc/rpc.h: include/rpc/rpc.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/rpc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/rpc.h

$(PROJECT_INCLUDE)/rpc/rpc_com.h: include/rpc/rpc_com.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/rpc_com.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/rpc_com.h

$(PROJECT_INCLUDE)/rpc/rpc_msg.h: include/rpc/rpc_msg.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/rpc_msg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/rpc_msg.h

$(PROJECT_INCLUDE)/rpc/svc.h: include/rpc/svc.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/svc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/svc.h

$(PROJECT_INCLUDE)/rpc/svc_auth.h: include/rpc/svc_auth.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/svc_auth.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/svc_auth.h

$(PROJECT_INCLUDE)/rpc/types.h: include/rpc/types.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/types.h

$(PROJECT_INCLUDE)/rpc/xdr.h: include/rpc/xdr.h $(PROJECT_INCLUDE)/rpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rpc/xdr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rpc/xdr.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
