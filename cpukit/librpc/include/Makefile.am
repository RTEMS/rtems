##
## $Id$
##

include_rpcdir = $(includedir)/rpc

if HAS_NETWORKING
include_rpc_HEADERS = \
    rpc/auth.h rpc/auth_des.h rpc/auth_unix.h rpc/clnt.h rpc/des.h rpc/des_crypt.h rpc/pmap_clnt.h \
    rpc/pmap_prot.h rpc/pmap_rmt.h rpc/rpc.h rpc/rpc_com.h rpc/rpc_msg.h rpc/svc.h rpc/svc_auth.h \
    rpc/types.h rpc/xdr.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rpc \
    $(include_rpc_HEADERS:rpc/%=$(PROJECT_INCLUDE)/rpc/%)

$(PROJECT_INCLUDE)/rpc:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = rpcsvc/bootparam_prot.x rpcsvc/crypt.x rpcsvc/key_prot.x rpcsvc/klm_prot.x rpcsvc/mount.x \
    rpcsvc/nfs_prot.x rpcsvc/nis.x rpcsvc/nis_cache.x rpcsvc/nis_callback.x rpcsvc/nis_db.h rpcsvc/nis_object.x \
    rpcsvc/nis_tags.h rpcsvc/nislib.h rpcsvc/nlm_prot.x rpcsvc/pmap_prot.x rpcsvc/rex.x rpcsvc/rnusers.x rpcsvc/rquota.x \
    rpcsvc/rstat.x rpcsvc/rwall.x rpcsvc/sm_inter.x rpcsvc/spray.x rpcsvc/yp.x rpcsvc/yp_prot.h rpcsvc/ypclnt.h rpcsvc/yppasswd.x \
    rpcsvc/ypupdate_prot.x rpcsvc/ypxfrd.x

include $(top_srcdir)/automake/local.am
