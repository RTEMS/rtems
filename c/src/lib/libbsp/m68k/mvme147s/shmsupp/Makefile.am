##
## $Id$
##

PGM = $(ARCH)/shmsupp.rel

C_FILES = addrconv.c getcfg.c lock.c mpisr.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

if HAS_MP
all-local: $(OBJS) $(PGM)
endif

.PRECIOUS: $(PGM)

EXTRA_DIST = addrconv.c getcfg.c lock.c mpisr.c

include $(top_srcdir)/../../../../automake/local.am
