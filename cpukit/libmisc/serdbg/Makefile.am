##
## $Id$
##

include_rtemsdir = $(includedir)/rtems

LIBNAME = libserdbg
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = serdbg.c serdbgio.c termios_printk.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

if LIBSERDBG
include_HEADERS = serdbgcnf.h serdbg.h termios_printk_cnf.h \
                  termios_printk.h
endif

noinst_HEADERS = config.h

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

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

if LIBSERDBG
all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB)
endif

.PRECIOUS: $(LIB)

EXTRA_DIST = README serdbg.c serdbgio.c serdbgcnf.h serdbg.h \
             termios_printk.c termios_printk.h termios_printk_cnf.h

include $(top_srcdir)/automake/local.am
