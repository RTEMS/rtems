##
## $Id$
##
include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems

noinst_LIBRARIES = libthumbisr.a
libthumbisr_a_SOURCES = thumb_isr.c
libthumbisr_a_CPPFLAGS = $(AM_CPPFLAGS)
#CFLAGS += -marm -mthumb-interwork

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
