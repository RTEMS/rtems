##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

C_FILES = cpu_install_intr_stack.c cpu_install_raw_isr.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

#
# (OPTIONAL) Add local stuff here using +=
#

if i960ca
all-local: $(ARCH) $(OBJS)
endif

EXTRA_DIST = cpu_install_intr_stack.c cpu_install_raw_isr.c

include $(top_srcdir)/../../../automake/local.am
