##
##  $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

lib_LIBRARIES = libschedsim.a

cpukitdir=$(srcdir)/../../../../cpukit
libschedsim_a_CPPFLAGS  = -D__RTEMS_VIOLATE_KERNEL_VISIBILITY__
libschedsim_a_CPPFLAGS += -I$(srcdir)/sched_cpu
libschedsim_a_CPPFLAGS += -I$(srcdir)/../../rtems
libschedsim_a_CPPFLAGS += -I$(cpukitdir)/include
libschedsim_a_CPPFLAGS += -I$(cpukitdir)/score/include
libschedsim_a_CPPFLAGS += -I$(cpukitdir)/score/inline
libschedsim_a_CPPFLAGS += -I$(cpukitdir)/rtems/include
libschedsim_a_CPPFLAGS += -I$(cpukitdir)/rtems/inline
libschedsim_a_CPPFLAGS += -I$(cpukitdir)/sapi/include
libschedsim_a_CPPFLAGS += -I$(cpukitdir)/sapi/inline
libschedsim_a_CPPFLAGS += -I$(cpukitdir)/libcsupport/include
libschedsim_a_CPPFLAGS += -I$(cpukitdir)/libmisc/stringto
libschedsim_a_CPPFLAGS += -I$(srcdir)/../../rtems/sched_cpu
libschedsim_a_CPPFLAGS += -I$(srcdir)/include


libschedsim_a_SOURCES = \
    commands.c \
    getopt.c \
    lookup_semaphore.c \
    lookup_task.c \
    main_echo.c \
    main_executing.c \
    main_heir.c \
    main_help.c \
    main_rtemsinit.c \
    main_clocktick.c \
    main_semcreate.c \
    main_semdelete.c \
    main_semflush.c \
    main_semobtain.c \
    main_semrelease.c \
    main_taskcreate.c \
    main_taskdelete.c \
    main_taskmode.c \
    main_taskpriority.c \
    main_taskresume.c \
    main_tasksuspend.c \
    main_taskwakeafter.c \
    shell_cmdset.c \
    shell_makeargs.c 

schedsim_shell_includedir = $(includedir)/schedsim
schedsim_newlib_includedir = $(includedir)/schedsim/newlib

schedsim_shell_include_HEADERS  = include/shell.h schedsim_shell.h

schedsim_newlib_include_HEADERS = \
  include/newlib/getopt.h \
  include/newlib/_ansi.h 

include $(top_srcdir)/../../automake/host.am
