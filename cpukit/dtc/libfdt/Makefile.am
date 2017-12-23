include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libfdt.a

libfdt_a_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)
libfdt_a_SOURCES =
libfdt_a_SOURCES += fdt_addresses.c
libfdt_a_SOURCES += fdt.c
libfdt_a_SOURCES += fdt_empty_tree.c
libfdt_a_SOURCES += fdt_ro.c
libfdt_a_SOURCES += fdt_rw.c
libfdt_a_SOURCES += fdt_strerror.c
libfdt_a_SOURCES += fdt_sw.c
libfdt_a_SOURCES += fdt_wip.c

include $(top_srcdir)/automake/local.am
