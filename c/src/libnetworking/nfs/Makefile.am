##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_nfsdir = $(includedir)/nfs

LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

C_FILES = bootp_subr.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D_COMPILING_BSD_KERNEL_ -DKERNEL -DINET -DNFS -DDIAGNOSTIC \
    -DBOOTP_COMPAT

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(ARCH) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = bootp_subr.c

include_nfs_HEADERS = krpc.h nfs.h nfsdiskless.h nfsproto.h rpcv2.h xdr_subs.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/nfs \
    $(include_nfs_HEADERS:%=$(PROJECT_INCLUDE)/nfs/%)

$(PROJECT_INCLUDE)/nfs:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/nfs/%.h: %.h
	$(INSTALL_DATA) $< $@

include $(top_srcdir)/../../../automake/local.am
