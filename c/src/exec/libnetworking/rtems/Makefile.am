##
## $Id$
##

include_rtemsdir = $(includedir)/rtems

LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

C_FILES = sghostname.c issetugid.c rtems_glue.c rtems_syscall.c \
    rtems_bootp.c rtems_showmbuf.c rtems_showroute.c rtems_showifstat.c \
    rtems_showipstat.c rtems_showicmpstat.c rtems_showtcpstat.c \
    rtems_showudpstat.c rtems_select.c mkrootfs.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include_rtems_HEADERS = rtems_bsdnet.h rtems_bsdnet_internal.h tftp.h \
    mkrootfs.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D_COMPILING_BSD_KERNEL_ -DKERNEL -DINET -DNFS -DDIAGNOSTIC \
    -DBOOTP_COMPAT

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(ARCH) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = issetugid.c rtems_bootp.c rtems_glue.c rtems_select.c \
    rtems_showicmpstat.c rtems_showifstat.c rtems_showipstat.c \
    rtems_showmbuf.c rtems_showroute.c rtems_showtcpstat.c \
    rtems_showudpstat.c rtems_syscall.c sghostname.c

include $(top_srcdir)/../../../../automake/local.am
