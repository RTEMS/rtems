##
## Makefile.am,v 1.5 2002/08/11 06:59:03 ralf Exp
##

C_FILES = idecfg.c pcmcia_ide.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

noinst_DATA = $(OBJS)

include $(top_srcdir)/../../../../automake/local.am
