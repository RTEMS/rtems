##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../mvme147/timer

PGM = $(ARCH)/timer.rel

C_FILES = timer.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = timerisr.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

include $(top_srcdir)/../../../../../../automake/local.am
