##
##  $Id$
##


SAMPLE = loopback
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = io event

C_FILES = init.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

DOCTYPES = scn
DOCS = $(DOCTYPES:%=$(SAMPLE).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/leaf.am
include $(top_srcdir)/sample.am

#
# (OPTIONAL) Add local stuff here using +=
#


if HAS_NETWORKING
all-local: ${ARCH} $(TMPINSTALL_FILES)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)
endif

EXTRA_DIST = $(C_FILES) $(DOCS)

include $(top_srcdir)/../../../../automake/local.am
