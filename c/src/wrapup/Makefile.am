##
##  $Id$
##
##  build and install "glommed" librtemsbsp.a
##

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../automake/compile.am
include $(top_srcdir)/../../automake/lib.am

LIB = $(PROJECT_RELEASE)/lib/librtemsbsp${LIB_VARIANT}.a

if HAS_NETWORKING
LIBNETWORKING = ../libnetworking/wrapup/$(ARCH)/libnetapps.a
endif

if HAS_RDBG
LIBRDBG = $(PROJECT_RELEASE)/lib/librdbg$(LIB_VARIANT).a
endif

if HAS_LIBBSP
LIBBSP = ../lib/libbsp/@RTEMS_CPU@/@RTEMS_BSP_FAMILY@/wrapup/$(ARCH)/libbsp.a
endif

if LIBCHIP
LIBCHIP = \
    ../libchip/rtc/$(ARCH)/librtcio.a \
    ../libchip/serial/$(ARCH)/libserialio.a \
    $(wildcard $(PROJECT_RELEASE)/lib/libnetchip$(LIB_VARIANT).a)
endif

SRCS = $(LIBBSP) $(LIBNETWORKING) $(LIBRDBG) \
    $(wildcard $(PROJECT_RELEASE)/lib/libcpu$(LIB_VARIANT).a) \
    $(LIBCHIP) \
    $(wildcard $(PROJECT_RELEASE)/lib/rtems-ctor$(LIB_VARIANT).$(OBJEXT))

TMPINSTALL_FILES += $(LIB)

all-local: $(ARCH) $(LIB) $(TMPINSTALL_FILES)

$(LIB): $(SRCS)
	rm -f $@
	rm -rf $(ARCH)/*
	test -d $(ARCH) || mkdir $(ARCH)
	for f in $(SRCS); do \
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
	$(AR) rc $@ $(ARCH)/*
	rm -f $(ARCH)/*
	$(RANLIB) $@

install-data-local:
	@$(mkinstalldirs) $(DESTDIR)$(bsplibdir)
	$(INSTALL_DATA) $(LIB) $(DESTDIR)$(bsplibdir)

include $(top_srcdir)/../../automake/local.am
