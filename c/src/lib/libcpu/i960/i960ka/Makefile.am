##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

C_FILES = cpu_install_intr_stack.c cpu_install_raw_isr.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

#
# (OPTIONAL) Add local stuff here using +=
#

if i960ka
all-local: $(ARCH) $(OBJS)
endif

EXTRA_DIST = cpu_install_intr_stack.c cpu_install_raw_isr.c

include $(top_srcdir)/../../../../../automake/local.am
