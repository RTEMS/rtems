##
## $Id$
##

SUBDIRS = . src

if HAS_NETWORKING
include_rpcdir = $(includedir)/rpc
include_rpc_HEADERS = \
    include/rpc/auth.h include/rpc/auth_des.h include/rpc/auth_unix.h \
    include/rpc/clnt.h include/rpc/des.h include/rpc/des_crypt.h \
    include/rpc/pmap_clnt.h include/rpc/pmap_prot.h include/rpc/pmap_rmt.h \
    include/rpc/rpc.h include/rpc/rpc_com.h include/rpc/rpc_msg.h \
    include/rpc/svc.h include/rpc/svc_auth.h \
    include/rpc/types.h include/rpc/xdr.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rpc \
    $(include_rpc_HEADERS:include/rpc/%.h=$(PROJECT_INCLUDE)/rpc/%.h)

$(PROJECT_INCLUDE)/rpc:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = include/rpcsvc/bootparam_prot.x include/rpcsvc/crypt.x \
    include/rpcsvc/key_prot.x include/rpcsvc/klm_prot.x \
    include/rpcsvc/mount.x include/rpcsvc/nfs_prot.x include/rpcsvc/nis.x \
    include/rpcsvc/nis_cache.x include/rpcsvc/nis_callback.x \
    include/rpcsvc/nis_db.h include/rpcsvc/nis_object.x \
    include/rpcsvc/nis_tags.h include/rpcsvc/nislib.h \
    include/rpcsvc/nlm_prot.x include/rpcsvc/pmap_prot.x \
    include/rpcsvc/rex.x include/rpcsvc/rnusers.x include/rpcsvc/rquota.x \
    include/rpcsvc/rstat.x include/rpcsvc/rwall.x include/rpcsvc/sm_inter.x \
    include/rpcsvc/spray.x include/rpcsvc/yp.x include/rpcsvc/yp_prot.h \
    include/rpcsvc/ypclnt.h include/rpcsvc/yppasswd.x \
    include/rpcsvc/ypupdate_prot.x include/rpcsvc/ypxfrd.x

EXTRA_DIST += README_RTEMS

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
