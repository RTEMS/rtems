##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = libxdr
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = xdr.c xdr_array.c xdr_float.c xdr_mem.c \
	xdr_rec.c xdr_reference.c xdr_sizeof.c xdr_stdio.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/lib.am

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a 

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -DIEEEFP '-D__P(x)=x'

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all-local: $(ARCH) $(OBJS) $(TMPINSTALL_FILES)

.PRECIOUS: $(LIB)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../automake/local.am
