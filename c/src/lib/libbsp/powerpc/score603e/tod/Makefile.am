##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/tod.rel

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

# generation 1
TOD_C_FILES_1_V = tod_g1.c
# generation 2
TOD_C_FILES_2_V = tod.c
TOD_C_FILES = $(TOD_C_FILES_$(SCORE603E_GENERATION)_V)

C_FILES = $(TOD_C_FILES)
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = tod.c tod_g1.c

include $(top_srcdir)/../../../../../../automake/local.am
