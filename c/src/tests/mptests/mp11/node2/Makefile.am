## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
 
VPATH = @srcdir@:@srcdir@/..

NODE = 2
TEST = mp11
PGM = ${ARCH}/$(TEST)-node$(NODE).exe

MANAGERS = io mp message partition semaphore

C_FILES = init.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = system.h

DOCTYPES = doc scn
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg
include $(top_srcdir)/mptests.am

if HAS_MP

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -DNODE_NUMBER=$(NODE) -I$(srcdir)/..

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all: $(ARCH) $(TMPINSTALL_FILES)
else
all:
endif

EXTRA_DIST = $(DOCS)

include $(top_srcdir)/../../../../automake/local.am
