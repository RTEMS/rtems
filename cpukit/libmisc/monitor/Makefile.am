##
## $Id$
##


include_rtemsdir = $(includedir)/rtems

LIBNAME = libmonitor
LIB = $(ARCH)/$(LIBNAME).a

# We only build multiprocessing related files if HAS_MP was defined
MP_C_FILES = mon-mpci.c

C_FILES = mon-command.c mon-symbols.c mon-prmisc.c mon-monitor.c \
    mon-object.c mon-server.c mon-task.c mon-queue.c mon-driver.c \
    mon-dname.c mon-itask.c mon-extension.c mon-manager.c mon-config.c

OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))
if HAS_MP
OBJS += $(MP_C_FILES:%.c=$(ARCH)/%.$(OBJEXT))
endif

include_rtems_HEADERS = monitor.h symbols.h

include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

$(PROJECT_INCLUDE)/rtems:
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = README mon-command.c mon-config.c mon-dname.c mon-driver.c \
    mon-extension.c mon-itask.c mon-manager.c mon-monitor.c mon-mpci.c \
    mon-object.c mon-prmisc.c mon-queue.c mon-server.c mon-symbols.c \
    mon-task.c

include $(top_srcdir)/../automake/local.am
