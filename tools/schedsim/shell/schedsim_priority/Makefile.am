##
## $Id$
##

bin_PROGRAMS = schedsim
schedsim_SOURCES = config.c \
    schedsim.cc wrap_thread_dispatch.c printheir_executing.c

cpukitdir=$(srcdir)/../../../../cpukit
schedsim_CPPFLAGS  = -I$(srcdir)/sched_cpu
schedsim_CPPFLAGS += -I$(srcdir)/../../rtems
schedsim_CPPFLAGS += -I$(cpukitdir)/include
schedsim_CPPFLAGS += -I$(cpukitdir)/score/include
schedsim_CPPFLAGS += -I$(cpukitdir)/score/inline
schedsim_CPPFLAGS += -I$(cpukitdir)/rtems/include
schedsim_CPPFLAGS += -I$(cpukitdir)/rtems/inline
schedsim_CPPFLAGS += -I$(cpukitdir)/sapi/include
schedsim_CPPFLAGS += -I$(cpukitdir)/sapi/inline
schedsim_CPPFLAGS += -I$(cpukitdir)/libcsupport/include
schedsim_CPPFLAGS += -I$(cpukitdir)/libmisc/stringto
schedsim_CPPFLAGS += -I$(srcdir)/../../rtems/sched_cpu
schedsim_CPPFLAGS += -I$(srcdir)/../shared/include
schedsim_CPPFLAGS += -I$(srcdir)/../shared

schedsim_LDFLAGS =-Wl,--wrap=_Thread_Dispatch
## schedsim_LDADD +=-Wl,--start-group
schedsim_LDADD = ../shared/libschedsim.a
schedsim_LDADD += ../../rtems/librtems.a
schedsim_LDADD += ../shared/libschedsim.a
## schedsim_LDADD +=-Wl,--end-group

include $(top_srcdir)/../../automake/host.am
