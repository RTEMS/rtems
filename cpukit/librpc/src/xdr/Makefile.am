##
## $Id$
##

LIB = $(ARCH)/libxdr.a

include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

C_FILES = xdr.c xdr_array.c xdr_float.c xdr_mem.c xdr_rec.c xdr_reference.c \
    xdr_sizeof.c xdr_stdio.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

noinst_MANS = xdr.3

#
# Add local stuff here using +=
#

AM_CPPFLAGS += '-D__P(x)=x'

$(LIB): $(OBJS)
	$(make-library)

if HAS_NETWORKING
man_MANS = $(noinst_MANS)
all-local: $(ARCH) $(LIB)
endif

EXTRA_DIST = $(C_FILES) $(noinst_MANS)

include $(top_srcdir)/../automake/local.am
