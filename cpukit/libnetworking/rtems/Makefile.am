##
## $Id$
##

include_rtemsdir = $(includedir)/rtems

LIB = $(ARCH)/lib.a

C_FILES = sghostname.c issetugid.c rtems_glue.c rtems_syscall.c \
    rtems_bootp.c rtems_showmbuf.c rtems_showroute.c rtems_showifstat.c \
    rtems_showipstat.c rtems_showicmpstat.c rtems_showtcpstat.c \
    rtems_showudpstat.c rtems_select.c mkrootfs.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_rtems_HEADERS = rtems_bsdnet.h rtems_bsdnet_internal.h tftp.h \
    ftpfs.h mkrootfs.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D_COMPILING_BSD_KERNEL_ -DKERNEL -DINET -DNFS -DDIAGNOSTIC \
    -DBOOTP_COMPAT

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(ARCH) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = issetugid.c rtems_bootp.c rtems_glue.c rtems_select.c \
    rtems_showicmpstat.c rtems_showifstat.c rtems_showipstat.c \
    rtems_showmbuf.c rtems_showroute.c rtems_showtcpstat.c \
    rtems_showudpstat.c rtems_syscall.c sghostname.c

include $(top_srcdir)/../automake/local.am
