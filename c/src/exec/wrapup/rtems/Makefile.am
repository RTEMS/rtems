##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = librtems
LIB = $(ARCH)/$(LIBNAME).a

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

CPU_OBJS = $(wildcard ../../score/cpu/$(RTEMS_CPU)/$(ARCH)/*.rel)
CORE_OBJS = $(wildcard ../../score/src/$(ARCH)/*.o)
SAPI_OBJS = $(wildcard ../../sapi/src/$(ARCH)/*.o)
RTEMS_OBJS = $(wildcard ../../rtems/src/$(ARCH)/*.o)

OBJS = $(CPU_OBJS) $(CORE_OBJS) $(RTEMS_OBJS) $(SAPI_OBJS)

SIZE_RTEMS = $(SHELL) $(PROJECT_TOPdir)/tools/cpu/generic/size_rtems

$(LIB): ${OBJS}
	$(make-library)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib$(MULTISUBDIR)/$(LIBNAME)$(LIB_VARIANT).a

$(PROJECT_RELEASE)/lib$(MULTISUBDIR)/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all-local: ${ARCH} $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
