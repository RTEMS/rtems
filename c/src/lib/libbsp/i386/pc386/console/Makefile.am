##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/../../shared/io

PGM = $(ARCH)/console.rel

C_FILES = console.c inch.c outch.c
S_FILES = videoAsm.S

console_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o) $(S_FILES:%.S=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(console_rel_OBJECTS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by
# 	../wrapup/Makefile

all-local: $(ARCH) $(console_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = console.c inch.c outch.c videoAsm.S

include $(top_srcdir)/../../../../../../automake/local.am
