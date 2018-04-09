include $(top_srcdir)/automake/compile.am

if LIBNETWORKING
project_lib_LIBRARIES = libnfs.a
libnfs_a_SOURCES =
libnfs_a_SOURCES += proto/mount_prot_xdr.c
libnfs_a_SOURCES += proto/nfs_prot_xdr.c
libnfs_a_SOURCES += src/nfs.c
libnfs_a_SOURCES += src/rpcio.c
libnfs_a_SOURCES += src/sock_mbuf.c
libnfs_a_SOURCES += src/xdr_mbuf.c
endif


if RPCTOOLS
$(srcdir)/proto/mount_prot.h: proto/mount_prot.x
	(cd $(srcdir)/proto && $(RPCGEN) -h -C mount_prot.x > mount_prot.h)

$(srcdir)/proto/mount_prot_xdr.c: proto/mount_prot.x proto/mount_prot.h
	(cd $(srcdir)/proto && $(RPCGEN) -c -C mount_prot.x \
	> mount_prot_xdr.c)

$(srcdir)/proto/nfs_prot.h: proto/nfs_prot.x
	(cd $(srcdir)/proto && $(RPCGEN) -h -C nfs_prot.x > nfs_prot.h)

$(srcdir)/proto/nfs_prot_xdr.c: proto/nfs_prot.x proto/nfs_prot.h
	(cd $(srcdir)/proto && $(RPCGEN) -c -C nfs_prot.x \
	> nfs_prot_xdr.c)
endif

include $(top_srcdir)/automake/local.am
