##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = librtemscpu
LIB = $(ARCH)/$(LIBNAME).a

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

CPU_OBJS = $(wildcard ../score/cpu/$(RTEMS_CPU)/$(ARCH)/*.rel)
CORE_OBJS = $(wildcard ../score/src/$(ARCH)/*.o)
SAPI_OBJS = $(wildcard ../sapi/src/$(ARCH)/*.o)
RTEMS_OBJS = $(wildcard ../rtems/src/$(ARCH)/*.o)

if HAS_POSIX
POSIX_OBJS = $(wildcard ../posix/src/$(ARCH)/*.o)
endif

if HAS_ITRON
ITRON_OBJS = $(wildcard ../itron/src/$(ARCH)/*.o)
endif

OBJS = $(CPU_OBJS) $(CORE_OBJS) $(RTEMS_OBJS) $(SAPI_OBJS) \
    $(POSIX_OBJS) $(ITRON_OBJS)

$(LIB): ${OBJS}
	$(make-library)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib$(MULTISUBDIR)/$(LIBNAME)$(LIB_VARIANT).a

$(PROJECT_RELEASE)/lib$(MULTISUBDIR)/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

install-hook: $(LIB)
	@$(mkinstalldirs) $(libdir)
	$(INSTALL_DATA) $(LIB) $(libdir)

all-local: ${ARCH} $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
