## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
SAMPLE = termios
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = io

# C source names, if any, go here -- minus the .c
C_FILES = init.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES =
noinst_HEADERS =$(H_FILES)

DOCTYPES =
DOCS = $(DOCTYPES:%=$(SAMPLE).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

PGM = ${ARCH}/$(SAMPLE).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg
include $(top_srcdir)/libtests.am

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(ARCH) $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES) $(DOCS)

include $(top_srcdir)/../../../../automake/local.am
