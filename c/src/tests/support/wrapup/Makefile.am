##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = libtest
LIB = $(ARCH)/$(LIBNAME).a

GENERIC_FILES = stubdr

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(GENERIC_FILES), ../$(piece)/$(ARCH)/$(piece).rel)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

$(LIB): ${OBJS}
	$(make-library)

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all-local: ${ARCH} $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
