##
## $Id$
##

LIB = $(project_libdir)$(MULTISUBDIR)/librtemscpu$(LIB_VARIANT).a

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

if LIBSCORECPU
CPU_OBJS = ../score/cpu/$(RTEMS_CPU)/$(ARCH)/libscorecpu.a
endif
CORE_OBJS = $(wildcard ../score/src/$(ARCH)/*.$(OBJEXT))
SAPI_OBJS = $(wildcard ../sapi/src/$(ARCH)/*.$(OBJEXT))
RTEMS_OBJS = $(wildcard ../rtems/src/$(ARCH)/*.$(OBJEXT))

if HAS_POSIX
POSIX_OBJS = ../posix/src/$(ARCH)/libposix.a
endif

if HAS_ITRON
ITRON_OBJS = ../itron/src/$(ARCH)/libitron.a
endif

LIBCSUPPORT_OBJS = ../libcsupport/$(ARCH)/libcsupport.a

LIBBLOCK_OBJS = ../libblock/src/$(ARCH)/libblock.a
if !UNIX
LIBDOSFS_OBJS = ../libfs/src/dosfs/$(ARCH)/libdosfs.a
endif
LIBIMFS_OBJS = ../libfs/src/imfs/$(ARCH)/libimfs.a

if HAS_NETWORKING
NETWORKING_OBJS = \
    ../libnetworking/wrapup/$(ARCH)/libnetworking.a
endif

if LIBRPC
LIBRPC_OBJS = \
    ../librpc/src/rpc/$(ARCH)/librpc.a \
    ../librpc/src/xdr/$(ARCH)/libxdr.a
endif

OBJS = $(CPU_OBJS) $(CORE_OBJS) $(RTEMS_OBJS) $(SAPI_OBJS) \
    $(POSIX_OBJS) $(ITRON_OBJS) $(LIBCSUPPORT_OBJS) \
    $(LIBBLOCK_OBJS) $(LIBDOSFS_OBJS) $(LIBIMFS_OBJS) \
    $(NETWORKING_OBJS) $(LIBRPC_OBJS)

$(LIB): ${OBJS}
	rm -f $@
	test -d $(ARCH) || mkdir $(ARCH)
	rm -rf $(ARCH)/*
	for f in $(OBJS); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
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
	test -d $(project_libdir)$(MULTISUBDIR) || $(mkinstalldirs) $(project_libdir)$(MULTISUBDIR)
	$(AR) rc $@ $(ARCH)/*
	rm -f $(ARCH)/*.$(OBJEXT)
	$(RANLIB) $@

TMPINSTALL_FILES += $(project_libdir)$(MULTISUBDIR)/librtemscpu$(LIB_VARIANT).a

cpulibdir = $(libdir)
cpulib_DATA = $(LIB)

all-local: ${ARCH} $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
