##
## $Id$
##


LIBNAME = librtemscpu
LIB = $(PROJECT_RELEASE)/lib$(MULTISUBDIR)/$(LIBNAME)$(LIB_VARIANT).a

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

LIBCSUPPORT_OBJS = $(wildcard ../libcsupport/src/$(ARCH)/*.o)

LIBBLOCK_OBJS = $(wildcard ../libblock/$(ARCH)/*.o)
if !UNIX
LIBDOSFS_OBJS = $(wildcard ../libfs/src/dosfs/$(ARCH)/*.o)
endif
LIBIMFS_OBJS = $(wildcard ../libfs/src/imfs/$(ARCH)/*.o)

if HAS_NETWORKING
NETWORKING_OBJS = \
    ../libnetworking/wrapup/$(ARCH)/libnetworking.a
endif

OBJS = $(CPU_OBJS) $(CORE_OBJS) $(RTEMS_OBJS) $(SAPI_OBJS) \
    $(POSIX_OBJS) $(ITRON_OBJS) $(LIBCSUPPORT_OBJS) \
    $(LIBBLOCK_OBJS) $(LIBDOSFS_OBJS) $(LIBIMFS_OBJS) \
    $(NETWORKING_OBJS)

$(LIB): ${OBJS}
	rm -f $@
	test -d $(ARCH) || mkdir $(ARCH)
	rm -rf $(ARCH)/*
	for f in $(OBJS); do \
	  case $$f in \
	  *.o | *.rel) \
	    if test -f $(ARCH)/`basename $$f`; then \
	      if cmp $$f $(ARCH)/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f $(ARCH)/; \
	      chmod a-w $(ARCH)/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd $(ARCH); \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	$(AR) rc $@ $(ARCH)/*
	rm -f $(ARCH)/*.o
	$(RANLIB) $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib$(MULTISUBDIR)/$(LIBNAME)$(LIB_VARIANT).a

#$(PROJECT_RELEASE)/lib$(MULTISUBDIR)/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
#	$(INSTALL_DATA) $< $@

install-hook: $(LIB)
	@$(mkinstalldirs) $(libdir)
	$(INSTALL_DATA) $(LIB) $(libdir)

all-local: ${ARCH} $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
