##
## $Id$
##


LIBNAME = libxdr
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = xdr.c xdr_array.c xdr_float.c xdr_mem.c xdr_rec.c xdr_reference.c \
    xdr_sizeof.c xdr_stdio.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

noinst_MANS = xdr.3

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

#
# Add local stuff here using +=
#

AM_CPPFLAGS += '-D__P(x)=x'

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

if HAS_NETWORKING
man_MANS = $(noinst_MANS)
all-local: $(ARCH) $(OBJS) $(TMPINSTALL_FILES)
endif

.PRECIOUS: $(LIB)

EXTRA_DIST = $(C_FILES) $(noinst_MANS)

include $(top_srcdir)/../../../automake/local.am
