##
## $Id$
##

PGM = $(ARCH)/interrupts.rel

C_FILES = installisrentries.c vectorexceptions.c

S_FILES = isr_entries.S

interrupts_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT)) \
    $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

if tx39
EXTRA_FLAGS = -DTX39
endif

if mongoosev
EXTRA_FLAGS = -DMONGOOSEV
endif

AM_CPPFLAGS += $(EXTRA_FLAGS)

$(PGM): $(interrupts_rel_OBJECTS)
	$(make-rel)

all-local: $(interrupts_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = maxvectors.c

include $(top_srcdir)/../../../automake/local.am
