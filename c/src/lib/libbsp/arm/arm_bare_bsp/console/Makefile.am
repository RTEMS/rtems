##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared/comm:@srcdir@/../../shared/io

PGM = $(ARCH)/console.rel

C_FILES = uart.c console.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

noinst_HEADERS = uart.h

console_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(console_rel_OBJECTS)
	$(make-rel)

all: $(ARCH) $(console_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

include $(top_srcdir)/../../../../../../automake/local.am
