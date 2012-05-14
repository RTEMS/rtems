include $(top_srcdir)/automake/compile.am

AM_CPPFLAGS += -DRTEMS_BSP=\"@RTEMS_BSP@\"

noinst_LIBRARIES = libsupport.a

libsupport_a_SOURCES = version.c
libsupport_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(top_srcdir)/automake/local.am
