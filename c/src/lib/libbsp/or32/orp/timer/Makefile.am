##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../../shared

PGM = $(ARCH)/timer.rel

C_FILES = timer.c timerisr.c
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

all-local: $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = timer.c timerisr.c

include $(top_srcdir)/../../../../automake/local.am
