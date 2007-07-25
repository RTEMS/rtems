#
#  $Id$
#

include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = librtemsnfs.a

# if you have CEXP set this variable to 'YES'
# and some "help" info will be compiled in.
librtemsnfs_a_SOURCES = src/rpcio.c src/rpcio.h
librtemsnfs_a_SOURCES += src/nfs.c src/sock_mbuf.c src/xdr_mbuf.c
librtemsnfs_a_SOURCES += proto/mount_prot.h proto/nfs_prot.h

# With CEXP
# librtemsnfs_a_SOURCES += dirutils.c rpcio.modini.c nfs.modini.c cexphelp.c

include_HEADERS = src/librtemsNfs.h

# PGMS=${ARCH}/nfs.obj ${ARCH}/rpcio.obj ${ARCH}/dirutils.obj 

librtemsnfs_a_CPPFLAGS = $(AM_CPPFLAGS)
librtemsnfs_a_CPPFLAGS += -I$(srcdir)/proto
librtemsnfs_a_CPPFLAGS += -UHAVE_CEXP

project_lib_PROGRAMS =
#%nfs.obj: %nfs.o %nfs.modini.o
#	$(LD) -r -o $@ $^ -L../proto/$(ARCH) -lnfsprot
project_lib_PROGRAMS += nfs.rel
nfs_rel_SOURCES = src/nfs.c src/nfs.modini.c proto/nfs_prot.h proto/mount_prot.h
nfs_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/proto
nfs_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

#%rpcio.obj: %rpcio.o %sock_mbuf.o %xdr_mbuf.o %rpcio.modini.o
#	$(LD) -r -o $@ $^
project_lib_PROGRAMS += rpcio.rel
rpcio_rel_SOURCES = src/rpcio.c src/sock_mbuf.c src/xdr_mbuf.c src/rpcio.modini.c
rpcio_rel_CPPFLAGS = $(AM_CPPFLAGS)
rpcio_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)


#%dirutils.obj: %dirutils.o
#	$(LD) -r -o $@ $^
project_lib_PROGRAMS += dirutils.rel
dirutils_rel_SOURCES = src/dirutils.c
dirutils_rel_CPPFLAGS = $(AM_CPPFLAGS)
dirutils_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)


noinst_LIBRARIES += libnfsprot.a

libnfsprot_a_SOURCES = proto/nfs_prot_xdr.c proto/nfs_prot.h
libnfsprot_a_SOURCES += proto/mount_prot_xdr.c proto/mount_prot.h

libnfsprot_a_CPPFLAGS = $(AM_CPPFLAGS)
libnfsprot_a_CPPFLAGS += -I$(srcdir)/proto
libnfsprot_a_CPPFLAGS += -UHAVE_CEXP

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
