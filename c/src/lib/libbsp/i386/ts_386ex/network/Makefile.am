##
## $Id$
##


PGM = $(ARCH)/network.rel

C_FILES = ne2000.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -D_COMPILING_BSD_KERNEL_ -DKERNEL -DINET -DNFS -DDIAGNOSTIC \
    -DBOOTP_COMPAT

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

if HAS_NETWORKING
all-local: $(ARCH) $(OBJS) $(PGM)
endif

.PRECIOUS: $(PGM)

EXTRA_DIST = ne2000.c

include $(top_srcdir)/../../../../../../automake/local.am
