##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/..

NODE = 1
SAMPLE = base_mp-node$(NODE)
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = io mp

# C source names, if any, go here -- minus the .c
C_FILES = init.c apptask.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = system.h

DOCTYPES = doc scn
DOCS = $(DOCTYPES:%=base_mp.%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg
include $(top_srcdir)/sample.am

if HAS_MP

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -DNODE_NUMBER=$(NODE) -I.

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(ARCH) $(TMPINSTALL_FILES)
else
all-local:
endif

EXTRA_DIST = $(DOCS)

include $(top_srcdir)/../../../../automake/local.am
