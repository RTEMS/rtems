include $(top_srcdir)/automake/compile.am

include_HEADERS =

noinst_LIBRARIES = libstdthreads.a

libstdthreads_a_CFLAGS = -std=c11
libstdthreads_a_CPPFLAGS = $(AM_CPPFLAGS)

libstdthreads_a_SOURCES =
if HAVE_THREADS_H
libstdthreads_a_SOURCES += call_once.c
libstdthreads_a_SOURCES += cnd.c
libstdthreads_a_SOURCES += mtx.c
if HAS_PTHREADS
libstdthreads_a_SOURCES += thrd.c
endif
libstdthreads_a_SOURCES += tss.c
endif

include $(top_srcdir)/automake/local.am
