##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtemsdir = $(includedir)/rtems

LIBNAME = libserdbg-tmp
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = serdbg.c serdbgio.c termios_printk.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

noinst_HEADERS = 
include_HEADERS = serdbgcnf.h serdbg.h termios_printk_cnf.h \
                  termios_printk.h

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = README serdbg.c serdbgio.c serdbgcnf.h serdbg.h \
             termios_printk.c termios_printk.h termios_printk_cnf.h

include $(top_srcdir)/../../../automake/local.am
