## 
## Makefile,v 1.2 1995/05/31 16:56:37 joel Exp
## 
## @(#)Makefile        03/26/96     1.2

AUTOMAKE_OPTIONS = foreign 1.4

PGM=$(ARCH)/timer.rel

# NOTE: timerisr is normally an assembly file!!!

C_FILES = timer.c timerisr.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(SRCS) $(OBJS)
	$(make-rel)

all-local: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = Makefile gettime.S timer.c timerisr.c

include $(top_srcdir)/../../../../../automake/local.am
