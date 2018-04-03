include $(top_srcdir)/automake/compile.am

## shmdr
if HAS_MP
noinst_LIBRARIES = libshmdr.a
libshmdr_a_CPPFLAGS = $(AM_CPPFLAGS)
libshmdr_a_SOURCES = shmdr/addlq.c shmdr/cnvpkt.c shmdr/getlq.c shmdr/dump.c \
    shmdr/fatal.c shmdr/getpkt.c shmdr/init.c shmdr/initlq.c shmdr/intr.c \
    shmdr/mpisr.c shmdr/poll.c shmdr/receive.c shmdr/retpkt.c shmdr/send.c
endif


## --

include $(top_srcdir)/automake/local.am
