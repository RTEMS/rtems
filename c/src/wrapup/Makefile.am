##
##  $Id$
##
##  build and install "glommed" librtemsbsp.a
##

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

project_lib_DATA = $(ARCH)/librtemsbsp$(LIB_VARIANT).a

SRCS = ../support/${ARCH}/libsupport.a

if HAS_LIBBSP
SRCS += \
    ../lib/libbsp/@RTEMS_CPU@/@RTEMS_BSP_FAMILY@/wrapup/$(ARCH)/libbsp.a
endif

if HAS_NETWORKING
if HAS_POSIX
SRCS += ../libnetworking/rtems_webserver/libhttpd$(LIB_VARIANT).a
endif
SRCS += ../libnetworking/pppd/libpppd$(LIB_VARIANT).a
SRCS += ../libnetworking/rtems_servers/libftpd$(LIB_VARIANT).a
SRCS += ../libnetworking/rtems_telnetd/libtelnetd$(LIB_VARIANT).a
endif

if HAS_RDBG
SRCS += ../librdbg/src/$(RTEMS_CPU)/$(ARCH)/librdbg.a
endif

SRCS += $(wildcard $(PROJECT_LIB)/libcpu$(LIB_VARIANT).a)

if LIBCHIP
SRCS += ../libchip/rtc/$(ARCH)/librtcio.a
SRCS += ../libchip/serial/$(ARCH)/libserialio.a
SRCS += ../libchip/ide/$(ARCH)/libide.a
if HAS_NETWORKING
SRCS += ../libchip/network/$(ARCH)/libnetchip$(LIB_VARIANT).a
endif
endif

SRCS += $(wildcard $(PROJECT_LIB)/lib/rtems-ctor$(LIB_VARIANT).$(OBJEXT))

if HAS_MP
SRCS += ../libchip/shmdr/$(ARCH)/shmdr.rel
endif

all-local: $(TMPINSTALL_FILES)

$(ARCH)/librtemsbsp$(LIB_VARIANT).a: $(SRCS) $(ARCH)/$(dirstamp)
	rm -rf $(ARCH)/*.a $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
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
	$(RANLIB) $@

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/librtemsbsp$(LIB_VARIANT).a: $(ARCH)/librtemsbsp$(LIB_VARIANT).a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/librtemsbsp$(LIB_VARIANT).a
TMPINSTALL_FILES += $(PROJECT_LIB)/librtemsbsp$(LIB_VARIANT).a

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
