##
##  $Id$
##
##  build and install "glommed" librtemsbsp.a
##

include $(top_srcdir)/../../automake/compile.am
include $(top_srcdir)/../../automake/lib.am

LIB = $(PROJECT_RELEASE)/lib/librtemsbsp${LIB_VARIANT}.a

SRCS = ../support/${ARCH}/libsupport.a

if HAS_LIBBSP
SRCS += ../lib/libbsp/@RTEMS_CPU@/@RTEMS_BSP_FAMILY@/wrapup/$(ARCH)/libbsp.a
endif

if HAS_NETWORKING
SRCS += ../libnetworking/wrapup/$(ARCH)/libnetapps.a
endif

if HAS_RDBG
SRCS += $(PROJECT_RELEASE)/lib/librdbg$(LIB_VARIANT).a
endif

SRCS += $(wildcard $(PROJECT_RELEASE)/lib/libcpu$(LIB_VARIANT).a)

if LIBCHIP
SRCS += ../libchip/rtc/$(ARCH)/librtcio.a
SRCS += ../libchip/serial/$(ARCH)/libserialio.a
SRCS += ../libchip/ide/$(ARCH)/libide.a
SRCS += $(wildcard $(PROJECT_RELEASE)/lib/libnetchip$(LIB_VARIANT).a)
endif

SRCS += $(wildcard $(PROJECT_RELEASE)/lib/rtems-ctor$(LIB_VARIANT).$(OBJEXT))

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
