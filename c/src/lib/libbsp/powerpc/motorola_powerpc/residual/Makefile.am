##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

# FIXME: This does not make much sense
noinst_HEADERS = $(top_srcdir)/../../powerpc/shared/residual/pnp.h \
    $(top_srcdir)/../../powerpc/shared/residual/residual.h

powerpc_shared_residual_C_FILES = \
    $(top_srcdir)/../../powerpc/shared/residual/residual.c
OBJS = \
    $(powerpc_shared_residual_C_FILES:$(top_srcdir)/../../powerpc/shared/residual/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../powerpc/shared/residual/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

noinst_DATA = $(OBJS)

include $(top_srcdir)/../../../../automake/local.am
