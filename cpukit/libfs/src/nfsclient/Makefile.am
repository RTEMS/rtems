include $(top_srcdir)/automake/compile.am

#%dirutils.obj: %dirutils.o
#	$(LD) -r -o $@ $^
project_lib_PROGRAMS = dirutils.rel
dirutils_rel_SOURCES = src/dirutils.c
dirutils_rel_CPPFLAGS = $(AM_CPPFLAGS)
dirutils_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if LIBNETWORKING
project_lib_LIBRARIES = libnfs.a

# if you have CEXP set this variable to 'YES'
# and some "help" info will be compiled in.
libnfs_a_SOURCES = src/rpcio.c src/rpcio.h
libnfs_a_SOURCES += src/nfs.c src/sock_mbuf.c src/xdr_mbuf.c
libnfs_a_SOURCES += proto/mount_prot.h proto/nfs_prot.h

# With CEXP
# libnfs_a_SOURCES += dirutils.c rpcio.modini.c nfs.modini.c cexphelp.c

include_HEADERS = src/librtemsNfs.h

# PGMS=${ARCH}/nfs.obj ${ARCH}/rpcio.obj ${ARCH}/dirutils.obj

libnfs_a_CPPFLAGS = $(AM_CPPFLAGS)
libnfs_a_CPPFLAGS += -I$(srcdir)/proto
libnfs_a_CPPFLAGS += -UHAVE_CEXP

#%nfs.obj: %nfs.o %nfs.modini.o
#	$(LD) -r -o $@ $^ -L../proto/$(ARCH) -lnfsprot
project_lib_PROGRAMS += nfs.rel
nfs_rel_SOURCES = src/nfs.c proto/nfs_prot.h proto/mount_prot.h
nfs_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/proto
nfs_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
nfs_rel_LDADD = libnfs.a

#%rpcio.obj: %rpcio.o %sock_mbuf.o %xdr_mbuf.o %rpcio.modini.o
#	$(LD) -r -o $@ $^
project_lib_PROGRAMS += rpcio.rel
rpcio_rel_SOURCES = src/rpcio.c src/sock_mbuf.c src/xdr_mbuf.c
rpcio_rel_CPPFLAGS = $(AM_CPPFLAGS)
rpcio_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

libnfs_a_SOURCES += proto/nfs_prot_xdr.c proto/nfs_prot.h
libnfs_a_SOURCES += proto/mount_prot_xdr.c proto/mount_prot.h

libnfs_a_CPPFLAGS += -I$(srcdir)/proto
libnfs_a_CPPFLAGS += -UHAVE_CEXP
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

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
