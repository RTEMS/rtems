##
## $Id$
##


C_FILES = memcpy.c m68kidle.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

# gcc doesn't recognize difference between the cpu32 and cpu32+ so we have to
if mcpu32p
M68K_CPPFLAGS = -D__mcpu32p__
endif

if mcf5272
M68K_CPPFLAGS = -Dmcf5272
endif

AM_CPPFLAGS += $(M68K_CPPFLAGS)

all-local: $(ARCH) $(OBJS)

EXTRA_DIST = memcpy.c m68kidle.c

include $(top_srcdir)/../../../../../automake/local.am
