##
## $Id$
##

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

AM_CPPFLAGS += -DRTEMS_BSP=\"$(RTEMS_BSP)\"

C_FILES=version.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

LIB = $(ARCH)/libsupport.a

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(LIB)

include $(top_srcdir)/automake/local.am
