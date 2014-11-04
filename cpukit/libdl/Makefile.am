if LIBDL

include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libdl.a
libdl_a_SOURCES = \
  dlfcn-shell.c \
  dlfcn.c \
  fastlz.c \
  rap-shell.c \
  rap.c \
  rtl.c \
  rtl-alloc-heap.c \
  rtl-allocator.c \
  rtl-chain-iterator.c \
  rtl-debugger.c \
  rtl-elf.c \
  rtl-error.c \
  rtl-find-file.c \
  rtl-obj-cache.c \
  rtl-obj-comp.c \
  rtl-obj.c \
  rtl-rap.c \
  rtl-shell.c \
  rtl-string.c \
  rtl-sym.c \
  rtl-trace.c \
  rtl-unresolved.c

libdl_a_SOURCES += rtl-mdreloc-@RTEMS_CPU@.c
libdl_a_CPPFLAGS = $(AM_CPPFLAGS) -DRTEMS_RTL_RAP_LOADER=1 -DRTEMS_RTL_ELF_LOADER=1

endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
