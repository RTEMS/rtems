@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@c The following macros from confdefs.h have not been discussed in this
@c chapter:
@c 
@c CONFIGURE_NEWLIB_EXTENSION - probably not needed
@c CONFIGURE_MALLOC_REGION - probably not needed
@c CONFIGURE_LIBIO_SEMAPHORES - implicitly discussed.
@c CONFIGURE_INTERRUPT_STACK_MEMORY
@c CONFIGURE_GNAT_RTEMS
@c     CONFIGURE_GNAT_MUTEXES
@c     CONFIGURE_GNAT_KEYS
@c     CONFIGURE_MAXIMUM_ADA_TASKS
@c     CONFIGURE_MAXIMUM_FAKE_ADA_TASKS
@c     CONFIGURE_ADA_TASKS_STACK
@c 
@c In addition, there should be examples of defining your own
@c Device Driver Table, Init task table, etc.
@c
@c Regardless, this is a big step up. :)
@c 

@chapter Configuring a System

@section Introduction

RTEMS must be configured for an application.  This configuration
information encompasses a variety of information including 
the length of each clock tick, the maximum number of each RTEMS
object that can be created, the application initialization tasks,
and the device drivers in the application.  This information
is placed in data structures that are given to RTEMS at
system initialization time.  This chapter details the 
format of these data structures as well as a simpler
mechanism to automate the generation of these structures.


@section Automatic Generation of System Configuration

@cindex confdefs.h
@findex confdefs.h

RTEMS provides the @code{confdefs.h} C language header file that
based on the setting of a variety of macros can automatically
produce nearly all of the configuration tables required
by an RTEMS application.  Rather than building the individual
tables by hand, the application simply specifies the values
for the configuration parameters it wishes to set.  In the following
example, the configuration information for a simple system with
a message queue and a time slice of 50 milliseconds is configured:

@example
@group
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK   1000 /* 1 millisecond */
#define CONFIGURE_TICKS_PER_TIMESLICE       50 /* 50 milliseconds */

#define CONFIGURE_MAXIMUM_TASKS 4
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
@end group
@end example

This system will begin execution with the single initialization task
named @code{Init}.  It will be configured to have both a console
device driver (for standard I/O) and a clock tick device driver.

For each configuration parameter in the configuration tables, the
macro corresponding to that field is discussed.  Most systems
can be easily configured using the @code{confdefs.h} mechanism.

The @code{CONFIGURE_INIT} constant must be defined in order to
make @code{confdefs.h} instantiate the configuration data
structures.  This can only be defined in one source file per
application that includes @code{confdefs.h} or the symbol
table will be instantiated multiple times and linking errors
produced.

The user should be aware that the defaults are intentionally
set as low as possible.  By default, no application resources
are configured.  The @code{confdefs.h} file ensures that
at least one application tasks or thread is configured
and that at least one of the initialization task/thread
tables is configured.

The @code{confdefs.h} file estimates the amount of 
memory required for the RTEMS Executive Workspace.  This
estimate is only as accurate as the information given
to @code{confdefs.h} and may be either too high or too
low for a variety of reasons.  Some of the reasons that
@code{confdefs.h} may reserve too much memory for RTEMS
are:

@itemize @bullet
@item All tasks/threads are assumed to be floating point.
@end itemize

Conversely, there are many more reasons, the resource
estimate could be too low:

@itemize @bullet
@item Task/thread stacks greater than minimum size must be
accounted for explicitly by developer.

@item Memory for messages is not included.

@item Device driver requirements are not included.


@item Network stack requirements are not included.

@item Requirements for add-on libraries are not included.
@end itemize

In general, @code{confdefs.h} is very accurate when given
enough information.  However, it is quite easy to use
a library and not account for its resources.

The following subsection list all of the constants which can be
set by the user.

@subsection Library Support Definitions

This section defines the file system and IO library
related configuration parameters supported by
@code{confdefs.h}.

@itemize @bullet
@findex CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS
@item @code{CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS} is set to the
maximum number of files that can be concurrently open.  Libio requires
a Classic RTEMS semaphore for each file descriptor as well as one 
global one.  The default value is 3 file descriptors which is
enough to support standard input, output, and error output.

@findex CONFIGURE_TERMIOS_DISABLED
@item @code{CONFIGURE_TERMIOS_DISABLED} is defined if the
software implementing POSIX termios functionality is
not going to be used by this application.  By default, this
is not defined and resources are reserved for the
termios functionality.

@findex CONFIGURE_NUMBER_OF_TERMIOS_PORTS
@item @code{CONFIGURE_NUMBER_OF_TERMIOS_PORTS} is set to the
number of ports using the termios functionality.  Each 
concurrently active termios port requires resources.
By default, this is set to 1 so a console port can be
used.

@findex CONFIGURE_HAS_OWN_MOUNT_TABLE
@item @code{CONFIGURE_HAS_OWN_MOUNT_TABLE} is defined when the
application provides their own filesystem mount table.  The
mount table is an array of @code{rtems_filesystem_mount_table_t}
entries pointed to by the global variable
@code{rtems_filesystem_mount_table}.  The number of
entries in this table is in an integer variable named
@code{rtems_filesystem_mount_table_t}.

@findex CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
@item @code{CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM} is defined
if the application wishes to use the full functionality
IMFS.  By default, the miniIMFS is used.  The miniIMFS
is a minimal functionality subset of the In-Memory
FileSystem (IMFS).  The miniIMFS is comparable
in functionality to the pseudo-filesystem name space provided 
before RTEMS release 4.5.0.  The miniIMFS supports
only directories and device nodes and is smaller in executable
code size than the full IMFS.

@findex STACK_CHECKER_ON
@item @code{STACK_CHECKER_ON} is defined when the application 
wishes to enable run-time stack bounds checking.  This increases
the time required to create tasks as well as adding overhead
to each context switch.  By default, this is not defined and
thus stack checking is disabled.

@end itemize

@subsection Basic System Information

This section defines the general system configuration parameters supported by
@code{confdefs.h}.

@itemize @bullet
@findex CONFIGURE_HAS_OWN_CONFIGURATION_TABLE
@item @code{CONFIGURE_HAS_OWN_CONFIGURATION_TABLE} should only be defined
if the application is providing their own complete set of configuration 
tables.

@findex CONFIGURE_INTERRUPT_STACK_MEMORY
@item @code{CONFIGURE_INTERRUPT_STACK_MEMORY} is set to the 
size of the interrupt stack.  The interrupt stack size is
usually set by the BSP but since this memory is allocated
from the RTEMS Ram Workspace, it must be accounted for.  The
default for this field is RTEMS_MINIMUM_STACK_SIZE.  [NOTE:
At this time, changing this constant does NOT change the
size of the interrupt stack, only the amount of memory
reserved for it.]

@findex CONFIGURE_EXECUTIVE_RAM_WORK_AREA
@item @code{CONFIGURE_EXECUTIVE_RAM_WORK_AREA} is the base 
address of the RTEMS RAM Workspace.  By default, this value
is NULL indicating that the BSP is to determine the location
of the RTEMS RAM Workspace.

@findex CONFIGURE_MICROSECONDS_PER_TICK
@item @code{CONFIGURE_MICROSECONDS_PER_TICK} is the length
of time between clock ticks.  By default, this is set to
10000 microseconds.

@findex CONFIGURE_TICKS_PER_TIMESLICE
@item @code{CONFIGURE_TICKS_PER_TIMESLICE} is the number
of ticks per each task's timeslice.  By default, this is
50.

@findex CONFIGURE_MEMORY_OVERHEAD
@item @code{CONFIGURE_MEMORY_OVERHEAD} is set to the number of
bytes the applications wishes to add to the requirements calculated
by @code{confdefs.h}.  The default value is 0.

@findex CONFIGURE_EXTRA_TASK_STACKS
@item @code{CONFIGURE_EXTRA_TASK_STACKS} is set to the number of
bytes the applications wishes to add to the task stack requirements
calculated by @code{confdefs.h}.  This parameter is very important.
If the application creates tasks with stacks larger then the 
minimum, then that memory is NOT accounted for by @code{confdefs.h}.
The default value is 0.

@end itemize

NOTE: The required size of the Executive RAM Work Area is calculated
automatically when using the @code{confdefs.h} mechanism.

@subsection Device Driver Table

This section defines the configuration parameters related
to the automatic generation of a Device Driver Table.  As
@code{confdefs.h} only is aware of a small set of 
standard device drivers, the generated Device Driver
Table is suitable for simple applications with no
custom device drivers.

@itemize @bullet
@findex CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE
@item @code{CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE} is defined if
the application wishes to provide their own Device Driver Table.
The table generated is an array of @code{rtems_driver_address_table}
entries named @code{Device_drivers}.  By default, this is not
defined indicating the @code{confdefs.h} is providing the
device driver table.

@findex CONFIGURE_MAXIMUM_DEVICES
@item @code{CONFIGURE_MAXIMUM_DEVICES} is defined
to the number of individual devices that may be registered
in the system.  By default, this is set to 20.

@findex CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
@item @code{CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER}
is defined
if the application wishes to include the Console Device Driver.
This device driver is responsible for providing standard input
and output using "/dev/console".  By default, this is not
defined.

@findex CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
@item @code{CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER}
is defined
if the application wishes to include the Console Device Driver.
This device driver is responsible for providing standard input
and output using "/dev/console".  By default, this is not
defined.

@findex CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER
@item @code{CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER}
is defined if the application wishes to include the Timer Driver.
This device driver is used to benchmark execution times
by the RTEMS Timing Test Suites.  By default, this is not
defined.

@c @item @code{CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER}
@c is defined
@c if the application wishes to include the Real-Time Clock Driver.
@c By default, this is not defined.

@findex CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
@item @code{CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER}
is defined if the application wishes to include the Stub Device Driver.
This device driver simply provides entry points that return
successful and is primarily a test fixture.
By default, this is not defined.

@end itemize

@subsection Multiprocessing Configuration

This section defines the multiprocessing related
system configuration parameters supported by @code{confdefs.h}.
This class of Configuration Constants are only applicable if
@code{CONFIGURE_MP_APPLICATION} is defined.

@itemize @bullet
@findex CONFIGURE_HAS_OWN_MULTIPROCESING_TABLE
@item @code{CONFIGURE_HAS_OWN_MULTIPROCESING_TABLE} is defined
if the application wishes to provide their own Multiprocessing
Configuration Table.  The generated table is named
@code{Multiprocessing_configuration}.  By default, this
is not defined.

@findex CONFIGURE_MP_NODE_NUMBER
@item @code{CONFIGURE_MP_NODE_NUMBER} is the node number of
this node in a multiprocessor system.  The default node number
is @code{NODE_NUMBER} which is set directly in RTEMS test Makefiles.

@findex CONFIGURE_MP_MAXIMUM_NODES
@item @code{CONFIGURE_MP_MAXIMUM_NODES} is the maximum number
of nodes in a multiprocessor system.  The default is 2.

@findex CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS
@item @code{CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS}
is the maximum number
of concurrently active global objects in a multiprocessor
system.  The default is 32.

@findex CONFIGURE_MP_MAXIMUM_PROXIES
@item @code{CONFIGURE_MP_MAXIMUM_PROXIES} is the maximum number
of concurrently active thread/task proxies in a multiprocessor
system.  The default is 32.

@findex CONFIGURE_MP_MPCI_TABLE_POINTER
@item @code{CONFIGURE_MP_MPCI_TABLE_POINTER} is the pointer
to the MPCI Configuration Table.  The default value of
this field is @code{&MPCI_table}.
@end itemize

@subsection Classic API Configuration

This section defines the Classic API related
system configuration parameters supported by @code{confdefs.h}.

@itemize @bullet
@findex CONFIGURE_MAXIMUM_TASKS
@item @code{CONFIGURE_MAXIMUM_TASKS} is the maximum number of
Classic API tasks that can be concurrently active. 
The default for this field is 0.

@findex CONFIGURE_MAXIMUM_TIMERS
@item @code{CONFIGURE_MAXIMUM_TIMERS} is the maximum number of
Classic API timers that can be concurrently active. 
The default for this field is 0.

@findex CONFIGURE_MAXIMUM_SEMAPHORES
@item @code{CONFIGURE_MAXIMUM_SEMAPHORES} is the maximum number of
Classic API semaphores that can be concurrently active. 
The default for this field is 0.

@findex CONFIGURE_MAXIMUM_MESSAGE_QUEUES
@item @code{CONFIGURE_MAXIMUM_MESSAGE_QUEUES} is the maximum number of
Classic API message queues that can be concurrently active. 
The default for this field is 0.

@findex CONFIGURE_MAXIMUM_PARTITIONS
@item @code{CONFIGURE_MAXIMUM_PARTITIONS} is the maximum number of
Classic API partitions that can be concurrently active. 
The default for this field is 0.

@findex CONFIGURE_MAXIMUM_REGIONS
@item @code{CONFIGURE_MAXIMUM_REGIONS} is the maximum number of
Classic API regions that can be concurrently active. 
The default for this field is 0.

@findex CONFIGURE_MAXIMUM_PORTS
@item @code{CONFIGURE_MAXIMUM_PORTS} is the maximum number of
Classic API ports that can be concurrently active. 
The default for this field is 0.

@findex CONFIGURE_MAXIMUM_PERIODS
@item @code{CONFIGURE_MAXIMUM_PERIODS} is the maximum number of
Classic API rate monotonic periods that can be concurrently active. 
The default for this field is 0.

@findex CONFIGURE_MAXIMUM_USER_EXTENSIONS
@item @code{CONFIGURE_MAXIMUM_USER_EXTENSIONS} is the maximum number of
Classic API user extensions that can be concurrently active. 
The default for this field is 0.

@end itemize

@subsection Classic API Initialization Tasks Table Configuration

The @code{confdefs.h} configuration system can automatically
generate an Initialization Tasks Table named 
@code{Initialization_tasks} with a single entry.  The following
parameters control the generation of that table.

@itemize @bullet
@findex CONFIGURE_RTEMS_INIT_TASKS_TABLE
@item @code{CONFIGURE_RTEMS_INIT_TASKS_TABLE} is defined
if the user wishes to use a Classic RTEMS API Initialization
Task Table.  The application may choose to use the initialization
tasks or threads table from another API.  By default, this
field is not defined as the user MUST select their own
API for initialization tasks.

@findex CONFIGURE_HAS_OWN_INIT_TASK_TABLE
@item @code{CONFIGURE_HAS_OWN_INIT_TASK_TABLE} is defined
if the user wishes to define their own Classic API Initialization
Tasks Table.  This table should be named @code{Initialization_tasks}.
By default, this is not defined.

@findex CONFIGURE_INIT_TASK_NAME
@item @code{CONFIGURE_INIT_TASK_NAME} is the name
of the single initialization task defined by the
Classic API Initialization Tasks Table.  By default
the value is @code{rtems_build_name( 'U', 'I', '1', ' ' )}.

@findex CONFIGURE_INIT_TASK_STACK_SIZE
@item @code{CONFIGURE_INIT_TASK_STACK_SIZE}
is the stack size
of the single initialization task defined by the
Classic API Initialization Tasks Table.  By default
the value is @code{RTEMS_MINIMUM_STACK_SIZE}.

@findex CONFIGURE_INIT_TASK_PRIORITY
@item @code{CONFIGURE_INIT_TASK_PRIORITY}
is the initial priority
of the single initialization task defined by the
Classic API Initialization Tasks Table.  By default
the value is 1 which is the highest priority
in the Classic API.

@findex CONFIGURE_INIT_TASK_ATTRIBUTES
@item @code{CONFIGURE_INIT_TASK_ATTRIBUTES}
is the task attributes
of the single initialization task defined by the
Classic API Initialization Tasks Table.  By default
the value is @code{RTEMS_DEFAULT_ATTRIBUTES}.

@findex CONFIGURE_INIT_TASK_ENTRY_POINT
@item @code{CONFIGURE_INIT_TASK_ENTRY_POINT}
is the entry point (a.k.a. function name)
of the single initialization task defined by the
Classic API Initialization Tasks Table.  By default
the value is @code{Init}.

@findex CONFIGURE_INIT_TASK_INITIAL_MODES
@item @code{CONFIGURE_INIT_TASK_INITIAL_MODES}
is the initial execution mode
of the single initialization task defined by the
Classic API Initialization Tasks Table.  By default
the value is @code{RTEMS_NO_PREEMPT}.

@findex CONFIGURE_INIT_TASK_ARGUMENTS
@item @code{CONFIGURE_INIT_TASK_ARGUMENTS}
is the task argument
of the single initialization task defined by the
Classic API Initialization Tasks Table.  By default
the value is 0.

@end itemize


@subsection POSIX API Configuration

The parameters in this section are used to configure resources
for the RTEMS POSIX API.  They are only relevant if the POSIX API
is enabled at configure time using the @code{--enable-posix} option.

@itemize @bullet
@findex CONFIGURE_MAXIMUM_POSIX_THREADS
@item @code{CONFIGURE_MAXIMUM_POSIX_THREADS} is the maximum number of
POSIX API threads that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_POSIX_MUTEXES
@item @code{CONFIGURE_MAXIMUM_POSIX_MUTEXES} is the maximum number of
POSIX API mutexes that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES
@item @code{CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES} is the maximum number of
POSIX API condition variables that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_POSIX_KEYS
@item @code{CONFIGURE_MAXIMUM_POSIX_KEYS} is the maximum number of
POSIX API keys that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_POSIX_TIMERS
@item @code{CONFIGURE_MAXIMUM_POSIX_TIMERS} is the maximum number of
POSIX API timers that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS
@item @code{CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS} is the maximum number of
POSIX API queued signals that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES
@item @code{CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES} is the maximum number of
POSIX API message queues that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_POSIX_SEMAPHORES
@item @code{CONFIGURE_MAXIMUM_POSIX_SEMAPHORES} is the maximum number of
POSIX API semaphores that can be concurrently active.
The default is 0.

@end itemize

@subsection POSIX Initialization Threads Table Configuration

The @code{confdefs.h} configuration system can automatically
generate a POSIX Initialization Threads Table named 
@code{POSIX_Initialization_threads} with a single entry.  The following
parameters control the generation of that table.

@itemize @bullet
@findex CONFIGURE_POSIX_INIT_THREAD_TABLE
@item @code{CONFIGURE_POSIX_INIT_THREAD_TABLE}
is defined
if the user wishes to use a POSIX API Initialization
Threads Table.  The application may choose to use the initialization
tasks or threads table from another API.  By default, this
field is not defined as the user MUST select their own
API for initialization tasks.

@findex CONFIGURE_POSIX_HAS_OWN_INIT_THREAD_TABLE
@item @code{CONFIGURE_POSIX_HAS_OWN_INIT_THREAD_TABLE}
is defined if the user wishes to define their own POSIX API Initialization
Threads Table.  This table should be named @code{POSIX_Initialization_threads}.
By default, this is not defined.

@findex CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT
@item @code{CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT}
is the entry point (a.k.a. function name)
of the single initialization thread defined by the
POSIX API Initialization Threads Table.  By default
the value is @code{POSIX_Init}.

@findex CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
@item @code{CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE}
is the stack size of the single initialization thread defined by the
POSIX API Initialization Threads Table.  By default
the value is @code{RTEMS_MINIMUM_STACK_SIZE * 2}.

@end itemize

@subsection ITRON API Configuration

The parameters in this section are used to configure resources
for the RTEMS ITRON API.  They are only relevant if the POSIX API
is enabled at configure time using the @code{--enable-itron} option.

@itemize @bullet
@findex CONFIGURE_MAXIMUM_ITRON_TASKS
@item @code{CONFIGURE_MAXIMUM_ITRON_TASKS}
is the maximum number of
ITRON API tasks that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_ITRON_SEMAPHORES
@item @code{CONFIGURE_MAXIMUM_ITRON_SEMAPHORES}
is the maximum number of
ITRON API semaphores that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_ITRON_EVENTFLAGS
@item @code{CONFIGURE_MAXIMUM_ITRON_EVENTFLAGS}
is the maximum number of
ITRON API eventflags that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_ITRON_MAILBOXES
@item @code{CONFIGURE_MAXIMUM_ITRON_MAILBOXES}
is the maximum number of
ITRON API mailboxes that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS
@item @code{CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS}
is the maximum number of
ITRON API message buffers that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_ITRON_PORTS
@item @code{CONFIGURE_MAXIMUM_ITRON_PORTS}
is the maximum number of
ITRON API ports that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_ITRON_MEMORY_POOLS
@item @code{CONFIGURE_MAXIMUM_ITRON_MEMORY_POOLS}
is the maximum number of
ITRON API memory pools that can be concurrently active.
The default is 0.

@findex CONFIGURE_MAXIMUM_ITRON_FIXED_MEMORY_POOLS
@item @code{CONFIGURE_MAXIMUM_ITRON_FIXED_MEMORY_POOLS}
is the maximum number of
ITRON API fixed memory pools that can be concurrently active.
The default is 0.

@end itemize

@subsection ITRON Initialization Task Table Configuration

The @code{confdefs.h} configuration system can automatically
generate an ITRON Initialization Tasks Table named
@code{ITRON_Initialization_tasks} with a single entry.  The following
parameters control the generation of that table.

@itemize @bullet
@findex CONFIGURE_ITRON_INIT_TASK_TABLE
@item @code{CONFIGURE_ITRON_INIT_TASK_TABLE} is defined
if the user wishes to use a ITRON API Initialization
Tasks Table.  The application may choose to use the initialization
tasks or threads table from another API.  By default, this
field is not defined as the user MUST select their own
API for initialization tasks.

@findex CONFIGURE_ITRON_HAS_OWN_INIT_TASK_TABLE
@item @code{CONFIGURE_ITRON_HAS_OWN_INIT_TASK_TABLE}
is defined if the user wishes to define their own ITRON API Initialization
Tasks Table.  This table should be named @code{ITRON_Initialization_tasks}.
By default, this is not defined.

@findex CONFIGURE_ITRON_INIT_TASK_ENTRY_POINT
@item @code{CONFIGURE_ITRON_INIT_TASK_ENTRY_POINT}
is the entry point (a.k.a. function name)
of the single initialization task defined by the
ITRON API Initialization Tasks Table.  By default
the value is @code{ITRON_Init}.

@findex CONFIGURE_ITRON_INIT_TASK_ATTRIBUTES
@item @code{CONFIGURE_ITRON_INIT_TASK_ATTRIBUTES}
is the attribute set
of the single initialization task defined by the
ITRON API Initialization Tasks Table.  By default
the value is @code{TA_HLNG}.

@findex CONFIGURE_ITRON_INIT_TASK_PRIORITY
@item @code{CONFIGURE_ITRON_INIT_TASK_PRIORITY}
is the initial priority
of the single initialization task defined by the
ITRON API Initialization Tasks Table.  By default
the value is @code{1} which is the highest priority
in the ITRON API.

@findex CONFIGURE_ITRON_INIT_TASK_STACK_SIZE
@item @code{CONFIGURE_ITRON_INIT_TASK_STACK_SIZE}
is the stack size of the single initialization task defined by the
ITRON API Initialization Tasks Table.  By default
the value is @code{RTEMS_MINIMUM_STACK_SIZE}.

@end itemize

@subsection Ada Tasks

This section defines the system configuration parameters supported
by @code{confdefs.h} related to configuring RTEMS to support
a task using Ada tasking with GNAT.

@itemize @bullet
@findex CONFIGURE_GNAT_RTEMS
@item @code{CONFIGURE_GNAT_RTEMS} is defined to inform
RTEMS that the GNAT Ada run-time is to be used by the 
application.  This configuration parameter is critical
as it makes @code{confdefs.h} configure the resources
(mutexes and keys) used implicitly by the GNAT run-time.
By default, this parameter is not defined.

@findex CONFIGURE_MAXIMUM_ADA_TASKS
@item @code{CONFIGURE_MAXIMUM_ADA_TASKS} is the 
number of Ada tasks that can be concurrently active
in the system.  By default, when @code{CONFIGURE_GNAT_RTEMS}
is defined, this is set to 20.

@findex CONFIGURE_MAXIMUM_FAKE_ADA_TASKS
@item @code{CONFIGURE_MAXIMUM_FAKE_ADA_TASKS} is
the number of "fake" Ada tasks that can be concurrently
active in the system.  A "fake" Ada task is a non-Ada
task that makes calls back into Ada code and thus
implicitly uses the Ada run-time.

@end itemize

@section Configuration Table

@cindex Configuration Table
@cindex RTEMS Configuration Table

The RTEMS Configuration Table is used to tailor an
application for its specific needs.  For example, the user can
configure the number of device drivers or which APIs may be used.
THe address of the user-defined Configuration Table is passed as an
argument to the @code{@value{DIRPREFIX}initialize_executive}
directive, which MUST be the first RTEMS directive called.  
The RTEMS Configuration Table
is defined in the following @value{LANGUAGE} @value{STRUCTURE}:

@ifset is-C
@findex rtems_configuration_table
@example
@group
typedef struct @{
  void                             *work_space_start;
  rtems_unsigned32                  work_space_size;
  rtems_unsigned32                  maximum_extensions;
  rtems_unsigned32                  microseconds_per_tick;
  rtems_unsigned32                  ticks_per_timeslice;
  rtems_unsigned32                  maximum_devices;
  rtems_unsigned32                  number_of_device_drivers;
  rtems_driver_address_table       *Device_driver_table;
  rtems_unsigned32                  number_of_initial_extensions;
  rtems_extensions_table           *User_extension_table;
  rtems_multiprocessing_table      *User_multiprocessing_table;
  rtems_api_configuration_table    *RTEMS_api_configuration;
  posix_api_configuration_table    *POSIX_api_configuration;
@} rtems_configuration_table;
@end group
@end example
@end ifset

@ifset is-Ada
@example
type Configuration_Table is
   record
       Work_Space_Start             : RTEMS.Address;
       Work_Space_Size              : RTEMS.Unsigned32;
       Maximum_Extensions           : RTEMS.Unsigned32;
       Microseconds_Per_Tick        : RTEMS.Unsigned32;
       Ticks_Per_Timeslice          : RTEMS.Unsigned32;
       Maximum_Devices              : RTEMS.Unsigned32;
       Number_Of_Device_Drivers     : RTEMS.Unsigned32;
       Device_Driver_Table          :
            RTEMS.Driver_Address_Table_Pointer;
       Number_Of_Initial_Extensions : RTEMS.Unsigned32;
       User_Extension_Table         : RTEMS.Extensions_Table_Pointer;
       User_Multiprocessing_Table   :
            RTEMS.Multiprocessing_Table_Pointer;
       RTEMS_API_Configuration      :
            RTEMS.API_Configuration_Table_Pointer;
       POSIX_API_Configuration      :
            RTEMS.POSIX_API_Configuration_Table_Pointer;
   end record;

type Configuration_Table_Pointer is access all Configuration_Table;
@end example
@end ifset

@table @b
@item work_space_start
is the address of the RTEMS RAM Workspace.  
This area contains items such as the
various object control blocks (TCBs, QCBs, ...) and task stacks.
If the address is not aligned on a four-word boundary, then
RTEMS will invoke the fatal error handler during
@code{@value{DIRPREFIX}initialize_executive}.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_EXECUTIVE_RAM_WORK_AREA}
which defaults to @code{NULL}.  Normally, this field should be 
configured as @code{NULL} as BSPs will assign memory for the
RTEMS RAM Workspace as part of system initialization.

@item work_space_size
is the calculated size of the
RTEMS RAM Workspace.  The section Sizing the RTEMS RAM Workspace
details how to arrive at this number.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_EXECUTIVE_RAM_SIZE}
and is calculated based on the other system configuration settings.

@item microseconds_per_tick
is number of microseconds per clock tick.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MICROSECONDS_PER_TICK}.
If not defined by the application, then the
@code{CONFIGURE_MICROSECONDS_PER_TICK} macro defaults to 10000 
(10 milliseconds).

@item ticks_per_timeslice
is the number of clock ticks for a timeslice.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_TICKS_PER_TIMESLICE}.

@item maximum_devices
is the maximum number of devices that can be registered.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_DEVICES}.

@item number_of_device_drivers
is the number of device drivers for the system.  There should be
the same number of entries in the Device Driver Table.  If this field
is zero, then the @code{User_driver_address_table} entry should be NULL.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field is calculated
automatically based on the number of entries in the 
Device Driver Table.  This calculation is based on the assumption
that the Device Driver Table is named @code{Device_drivers}
and defined in C.  This table may be generated automatically
for simple applications using only the device drivers that correspond
to the following macros:

@itemize @bullet

@item @code{CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER}
@item @code{CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER}
@item @code{CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER}
@item @code{CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER}
@item @code{CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER}

@end itemize

Note that network device drivers are not configured in the
Device Driver Table.

@item Device_driver_table
is the address of the Device Driver Table.  This table contains the entry
points for each device driver.  If the number_of_device_drivers field is zero,
then this entry should be NULL. The format of this table will be
discussed below.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the Device Driver Table is assumed to be
named @code{Device_drivers} and defined in C.  If the application is providing
its own Device Driver Table, then the macro
@code{CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE} must be defined to indicate
this and prevent @code{confdefs.h} from generating the table.

@item number_of_initial_extensions
is the number of initial user extensions.  There should be
the same number of entries as in the User_extension_table.  If this field
is zero, then the User_driver_address_table entry should be NULL.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS}
which is set automatically by @code{confdefs.h} based on the size
of the User Extensions Table.

@item User_extension_table
is the address of the User
Extension Table.  This table contains the entry points for the
static set of optional user extensions.  If no user extensions
are configured, then this entry should be NULL.  The format of
this table will be discussed below.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the User Extensions Table is named
@code{Configuration_Initial_Extensions} and defined in
confdefs.h.  It is initialized based on the following
macros:

@itemize @bullet

@item @code{CONFIGURE_INITIAL_EXTENSIONS}
@item @code{STACK_CHECKER_EXTENSION}

@end itemize

The application may configure one or more initial user extension
sets by setting the @code{CONFIGURE_INITIAL_EXTENSIONS} macro.  By
defining the @code{STACK_CHECKER_EXTENSION} macro, the task stack bounds
checking user extension set is automatically included in the
application.

@item User_multiprocessing_table
is the address of the Multiprocessor Configuration Table.  This
table contains information needed by RTEMS only when used in a multiprocessor
configuration.  This field must be NULL when RTEMS is used in a
single processor configuration.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the Multiprocessor Configuration Table
is automatically generated when the @code{CONFIGURE_MP_APPLICATION}
is defined.  If @code{CONFIGURE_MP_APPLICATION} is not defined, the this
entry is set to NULL.  The generated table has the name
@code{Multiprocessing_configuration}.

@item RTEMS_api_configuration
is the address of the RTEMS API Configuration Table.  This table
contains information needed by the RTEMS API.  This field should be
NULL if the RTEMS API is not used.  [NOTE: Currently the RTEMS API
is required to support support components such as BSPs and libraries 
which use this API.]  This table is built automatically and this
entry filled in, if using the @code{confdefs.h} application
configuration mechanism.  The generated table has the name
@code{Configuration_RTEMS_API}.

@item POSIX_api_configuration
is the address of the POSIX API Configuration Table.  This table
contains information needed by the POSIX API.  This field should be
NULL if the POSIX API is not used.  This table is built automatically
and this entry filled in, if using the @code{confdefs.h} application
configuration mechanism.  The @code{confdefs.h} application 
mechanism will fill this field in with the address of the
@code{Configuration_POSIX_API} table of POSIX API is configured
and NULL if the POSIX API is not configured.

@end table

@section RTEMS API Configuration Table

@cindex RTEMS API Configuration Table

The RTEMS API Configuration Table is used to configure the 
managers which constitute the RTEMS API for a particular application.  
For example, the user can configure the maximum number of tasks for 
this application. The RTEMS API Configuration Table is defined in 
the following @value{LANGUAGE} @value{STRUCTURE}:

@ifset is-C
@findex rtems_api_configuration_table
@example
@group
typedef struct @{
  rtems_unsigned32                  maximum_tasks;
  rtems_unsigned32                  maximum_timers;
  rtems_unsigned32                  maximum_semaphores;
  rtems_unsigned32                  maximum_message_queues;
  rtems_unsigned32                  maximum_partitions;
  rtems_unsigned32                  maximum_regions;
  rtems_unsigned32                  maximum_ports;
  rtems_unsigned32                  maximum_periods;
  rtems_unsigned32                  number_of_initialization_tasks;
  rtems_initialization_tasks_table *User_initialization_tasks_table;
@} rtems_api_configuration_table;
@end group
@end example
@end ifset

@ifset is-Ada
@example
type API_Configuration_Table is
   record
      Maximum_Tasks                   : RTEMS.Unsigned32;
      Maximum_Timers                  : RTEMS.Unsigned32;
      Maximum_Semaphores              : RTEMS.Unsigned32;
      Maximum_Message_queues          : RTEMS.Unsigned32;
      Maximum_Partitions              : RTEMS.Unsigned32;
      Maximum_Regions                 : RTEMS.Unsigned32;
      Maximum_Ports                   : RTEMS.Unsigned32;
      Maximum_Periods                 : RTEMS.Unsigned32;
      Number_Of_Initialization_Tasks  : RTEMS.Unsigned32;
      User_Initialization_Tasks_Table :
           RTEMS.Initialization_Tasks_Table_Pointer;
   end record;

type API_Configuration_Table_Pointer is
           access all API_Configuration_Table;
@end example
@end ifset

@table @b
@item maximum_tasks
is the maximum number of tasks that
can be concurrently active (created) in the system including
initialization tasks.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_TASKS}.
If not defined by the application, then the @code{CONFIGURE_MAXIMUM_TASKS}
macro defaults to 10.

@item maximum_timers
is the maximum number of timers
that can be concurrently active in the system.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_TIMERS}.
If not defined by the application, then the @code{CONFIGURE_MAXIMUM_TIMERS}
macro defaults to 0.

@item maximum_semaphores
is the maximum number of
semaphores that can be concurrently active in the system.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_SEMAPHORES}.
If not defined by the application, then the @code{CONFIGURE_MAXIMUM_SEMAPHORES}
macro defaults to 0.

@item maximum_message_queues
is the maximum number of
message queues that can be concurrently active in the system.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_MESSAGE_QUEUES}.
If not defined by the application, then the
@code{CONFIGURE_MAXIMUM_MESSAGE_QUEUES} macro defaults to 0.

@item maximum_partitions
is the maximum number of
partitions that can be concurrently active in the system.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_PARTITIONS}.
If not defined by the application, then the @code{CONFIGURE_MAXIMUM_PARTITIONS}
macro defaults to 0.

@item maximum_regions
is the maximum number of regions
that can be concurrently active in the system.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_REGIONS}.
If not defined by the application, then the @code{CONFIGURE_MAXIMUM_REGIONS}
macro defaults to 0.

@item maximum_ports
is the maximum number of ports into
dual-port memory areas that can be concurrently active in the
system.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_PORTS}.
If not defined by the application, then the @code{CONFIGURE_MAXIMUM_PORTS}
macro defaults to 0.

@item number_of_initialization_tasks
is the number of initialization tasks configured.  At least one
RTEMS initialization task or POSIX initializatin must be configured
in order for the user's application to begin executing.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the user must define the
@code{CONFIGURE_RTEMS_INIT_TASKS_TABLE} to indicate that there
is one or more RTEMS initialization task.  If the application
only has one RTEMS initialization task, then the automatically
generated Initialization Task Table will be sufficient.  The following
macros correspond to the single initialization task:
  
@itemize @bullet

@item @code{CONFIGURE_INIT_TASK_NAME} - is the name of the task.  
If this macro is not defined by the application, then this defaults
to the task name of @code{"UI1 "} for User Initialization Task 1.

@item @code{CONFIGURE_INIT_TASK_STACK_SIZE} - is the stack size
of the single initialization task.  If this macro is not defined
by the application, then this defaults to @code{RTEMS_MINIMUM_STACK_SIZE}.

@item @code{CONFIGURE_INIT_TASK_PRIORITY} - is the initial priority
of the single initialization task.  If this macro is not defined
by the application, then this defaults to 1.

@item @code{CONFIGURE_INIT_TASK_ATTRIBUTES} - is the attributes
of the single initialization task.  If this macro is not defined
by the application, then this defaults to @code{RTEMS_DEFAULT_ATTRIBUTES}.

@item @code{CONFIGURE_INIT_TASK_ENTRY_POINT} - is the entry point
of the single initialization task.  If this macro is not defined
by the application, then this defaults to the C language routine
@code{Init}.

@item @code{CONFIGURE_INIT_TASK_INITIAL_MODES} - is the initial execution
modes of the single initialization task.  If this macro is not defined
by the application, then this defaults to @code{RTEMS_NO_PREEMPT}.

@item @code{CONFIGURE_INIT_TASK_ARGUMENTS} - is the argument passed to the 
of the single initialization task.  If this macro is not defined
by the application, then this defaults to 0.


@end itemize


has the option to have 
 value for this field corresponds
to the setting of the macro @code{}.

@item User_initialization_tasks_table
is the address of the Initialization Task Table. This table contains the
information needed to create and start each of the
initialization tasks.  The format of this table will be discussed below.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_EXECUTIVE_RAM_WORK_AREA}.

@end table

@section POSIX API Configuration Table

@cindex POSIX API Configuration Table

The POSIX API Configuration Table is used to configure the
managers which constitute the POSIX API for a particular application.
For example, the user can configure the maximum number of threads for
this application. The POSIX API Configuration Table is defined in
the following @value{LANGUAGE} @value{STRUCTURE}:
 
@ifset is-C
@findex posix_initialization_threads_table
@findex posix_api_configuration_table
@example
@group
typedef struct @{
  void       *(*thread_entry)(void *);
@} posix_initialization_threads_table;
 
typedef struct @{
  int                                 maximum_threads;
  int                                 maximum_mutexes;
  int                                 maximum_condition_variables;
  int                                 maximum_keys;
  int                                 maximum_timers;
  int                                 maximum_queued_signals;
  int                                 number_of_initialization_tasks;
  posix_initialization_threads_table *User_initialization_tasks_table;
@} posix_api_configuration_table;
@end group
@end example
@end ifset

@ifset is-Ada
@example
   type POSIX_Thread_Entry is access procedure (
      Argument : in     RTEMS.Address
   );

   type POSIX_Initialization_Threads_Table_Entry is
   record
      Thread_Entry : RTEMS.POSIX_Thread_Entry;
   end record;

   type POSIX_Initialization_Threads_Table is array
       ( RTEMS.Unsigned32 range <> ) of 
       RTEMS.POSIX_Initialization_Threads_Table_Entry;

   type POSIX_Initialization_Threads_Table_Pointer is access all
       POSIX_Initialization_Threads_Table; 

   type POSIX_API_Configuration_Table_Entry is
      record
         Maximum_Threads                 : Interfaces.C.Int;
         Maximum_Mutexes                 : Interfaces.C.Int;
         Maximum_Condition_Variables     : Interfaces.C.Int;
         Maximum_Keys                    : Interfaces.C.Int;
         Maximum_Timers                  : Interfaces.C.Int;
         Maximum_Queued_Signals          : Interfaces.C.Int;
         Number_Of_Initialization_Tasks  : Interfaces.C.Int;
         User_Initialization_Tasks_Table : 
            RTEMS.POSIX_Initialization_Threads_Table_Pointer;
      end record;

   type POSIX_API_Configuration_Table is array
      ( RTEMS.Unsigned32 range <> ) of
          RTEMS.POSIX_API_Configuration_Table_Entry;

   type POSIX_API_Configuration_Table_Pointer is access all
          RTEMS.POSIX_API_Configuration_Table;
@end example
@end ifset
 
@table @b
@item maximum_threads
is the maximum number of threads that
can be concurrently active (created) in the system including
initialization threads.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_POSIX_THREADS}.
If not defined by the application, then the
@code{CONFIGURE_MAXIMUM_POSIX_THREADS} macro defaults to 10.

@item maximum_mutexes
is the maximum number of mutexes that can be concurrently 
active in the system.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_POSIX_MUTEXES}.
If not defined by the application, then the
@code{CONFIGURE_MAXIMUM_POSIX_MUTEXES} macro defaults to 0.

@item maximum_condition_variables
is the maximum number of condition variables that can be 
concurrently active in the system.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES}.
If not defined by the application, then the
@code{CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES} macro defaults to 0.

@item maximum_keys
is the maximum number of keys that can be concurrently active in the system.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_POSIX_KEYS}.
If not defined by the application, then the
@code{CONFIGURE_MAXIMUM_POSIX_KEYS} macro defaults to 0.

@item maximum_timers
is the maximum number of POSIX timers that can be concurrently active
in the system.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_POSIX_TIMERS}.
If not defined by the application, then the
@code{CONFIGURE_MAXIMUM_POSIX_TIMERS} macro defaults to 0.

@item maximum_queued_signals
is the maximum number of queued signals that can be concurrently 
pending in the system.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS}.
If not defined by the application, then the
@code{CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS} macro defaults to 0.

@item number_of_initialization_threads
is the number of initialization threads configured.  At least one
initialization threads must be configured.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the user must define the
@code{CONFIGURE_POSIX_INIT_THREAD_TABLE} to indicate that there
is one or more POSIX initialization thread.  If the application
only has one POSIX initialization thread, then the automatically
generated POSIX Initialization Thread Table will be sufficient.  The following
macros correspond to the single initialization task:

@itemize @bullet

@item @code{CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT} - is the entry
point of the thread.  If this macro is not defined by the application,
then this defaults to the C routine @code{POSIX_Init}.

@item @code{CONFIGURE_POSIX_INIT_TASK_STACK_SIZE} - is the stack size
of the single initialization thread.  If this macro is not defined
by the application, then this defaults to
@code{(RTEMS_MINIMUM_STACK_SIZE * 2)}.

@end itemize
 
@item User_initialization_threads_table
is the address of the Initialization Threads Table. This table contains the
information needed to create and start each of the initialization threads.  
The format of each entry in this table is defined in the 
@code{posix_initialization_threads_table} @value{STRUCTURE}.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the address of the @code{POSIX_Initialization_threads} structure.

@end table

@section CPU Dependent Information Table

@cindex CPU Dependent Information Table

The CPU Dependent Information Table is used to
describe processor dependent information required by RTEMS.
This table is generally used to supply RTEMS with information
only known by the Board Support Package.  The contents of this
table are discussed in the CPU Dependent Information Table
chapter of the Applications Supplement document for a specific
target processor.

The @code{confdefs.h} mechanism does not support generating this
table.  It is normally filled in by the Board Support Package.

@section Initialization Task Table

@cindex Initialization Tasks Table

The Initialization Task Table is used to describe
each of the user initialization tasks to the Initialization
Manager.  The table contains one entry for each initialization
task the user wishes to create and start.  The fields of this
data structure directly correspond to arguments to the
@code{@value{DIRPREFIX}task_create} and
@code{@value{DIRPREFIX}task_start} directives.  The number of entries is
found in the @code{number_of_initialization_tasks} entry in the
Configuration Table.  

The format of each entry in the
Initialization Task Table is defined in the following @value{LANGUAGE}
@value{STRUCTURE}:

@ifset is-C
@findex rtems_initialization_tasks_table
@example
typedef struct @{
  rtems_name           name;
  rtems_unsigned32     stack_size;
  rtems_task_priority  initial_priority;
  rtems_attribute      attribute_set;
  rtems_task_entry     entry_point;
  rtems_mode           mode_set;
  rtems_task_argument  argument;
@} rtems_initialization_tasks_table;
@end example
@end ifset

@ifset is-Ada
@example
type Initialization_Tasks_Table_Entry is
   record
      Name             : RTEMS.Name;          -- task name
      Stack_Size       : RTEMS.Unsigned32;    -- task stack size
      Initial_Priority : RTEMS.Task_priority; -- task priority
      Attribute_Set    : RTEMS.Attribute;     -- task attributes
      Entry_Point      : RTEMS.Task_Entry;    -- task entry point
      Mode_Set         : RTEMS.Mode;          -- task initial mode
      Argument         : RTEMS.Unsigned32;    -- task argument
   end record;

type Initialization_Tasks_Table is array
    ( RTEMS.Unsigned32 range <> ) of
      RTEMS.Initialization_Tasks_Table_Entry;

type Initialization_Tasks_Table_Pointer is access all
     Initialization_Tasks_Table;
@end example
@end ifset

@table @b
@item name
is the name of this initialization task.

@item stack_size
is the size of the stack for this initialization task.

@item initial_priority
is the priority of this initialization task.

@item attribute_set
is the attribute set used during creation of this initialization task.

@item entry_point
is the address of the entry point of this initialization task.

@item mode_set
is the initial execution mode of this initialization task.

@item argument
is the initial argument for this initialization task.

@end table

A typical declaration for an Initialization Task Table might appear as follows:

@ifset is-C
@example
rtems_initialization_tasks_table
Initialization_tasks[2] = @{
   @{ INIT_1_NAME,
     1024,
     1,
     DEFAULT_ATTRIBUTES,
     Init_1,
     DEFAULT_MODES,
     1

   @},
   @{ INIT_2_NAME,
     1024,
     250,
     FLOATING_POINT,
     Init_2,
     NO_PREEMPT,
     2

   @}
@};
@end example
@end ifset

@ifset is-Ada
@example
Initialization_Tasks : aliased
  RTEMS.Initialization_Tasks_Table( 1 .. 2 ) := (
   (INIT_1_NAME,
    1024,
    1,
    RTEMS.Default_Attributes,
    Init_1'Access,
    RTEMS.Default_Modes,
    1),
   (INIT_2_NAME,
    1024,
    250,
    RTEMS.Floating_Point,
    Init_2'Access,
    RTEMS.No_Preempt,
    2)
);
@end example
@end ifset

@section Driver Address Table

@cindex Device Driver Table

The Device Driver Table is used to inform the I/O
Manager of the set of entry points for each device driver
configured in the system.  The table contains one entry for each
device driver required by the application.  The number of
entries is defined in the number_of_device_drivers entry in the
Configuration Table.  The format of each entry in the Device
Driver Table is defined in 
the following @value{LANGUAGE} @value{STRUCTURE}:

@ifset is-C
@findex rtems_driver_address_table
@example
typedef struct @{
  rtems_device_driver_entry initialization;
  rtems_device_driver_entry open;
  rtems_device_driver_entry close;
  rtems_device_driver_entry read;
  rtems_device_driver_entry write;
  rtems_device_driver_entry control;
@} rtems_driver_address_table;
@end example
@end ifset

@ifset is-Ada
@example
type Driver_Address_Table_Entry is
   record
      Initialization : RTEMS.Device_Driver_Entry;
      Open           : RTEMS.Device_Driver_Entry;
      Close          : RTEMS.Device_Driver_Entry;
      Read           : RTEMS.Device_Driver_Entry;
      Write          : RTEMS.Device_Driver_Entry;
      Control        : RTEMS.Device_Driver_Entry;
   end record;

type Driver_Address_Table is array ( RTEMS.Unsigned32 range <> ) of
  RTEMS.Driver_Address_Table_Entry;

type Driver_Address_Table_Pointer is access all Driver_Address_Table;
@end example
@end ifset

@table @b
@item initialization
is the address of the entry point called by
@code{@value{DIRPREFIX}io_initialize}
to initialize a device driver and its associated devices.

@item open
is the address of the entry point called by @code{@value{DIRPREFIX}io_open}.

@item close
is the address of the entry point called by @code{@value{DIRPREFIX}io_close}.

@item read
is the address of the entry point called by @code{@value{DIRPREFIX}io_read}.

@item write
is the address of the entry point called by @code{@value{DIRPREFIX}io_write}.

@item control
is the address of the entry point called by @code{@value{DIRPREFIX}io_control}.

@end table

Driver entry points configured as NULL will always
return a status code of @code{@value{RPREFIX}SUCCESSFUL}.  No user code will be
executed in this situation.

A typical declaration for a Device Driver Table might appear as follows:

@ifset is-C
@example
rtems_driver_address_table Driver_table[2] = @{
   @{ tty_initialize, tty_open,  tty_close,  /* major = 0 */
     tty_read,       tty_write, tty_control
   @},
   @{ lp_initialize, lp_open,    lp_close,   /* major = 1 */
     NULL,          lp_write,   lp_control
   @}
@};
@end example
@end ifset

@ifset is-Ada
@example
@end example
@end ifset

More information regarding the construction and
operation of device drivers is provided in the I/O Manager
chapter.

@section User Extensions Table

@cindex User Extensions Table

The User Extensions Table is used to inform RTEMS of
the optional user-supplied static extension set.  This table
contains one entry for each possible extension.  The entries are
called at critical times in the life of the system and
individual tasks.  The application may create dynamic extensions
in addition to this single static set.  The format of each entry
in the User Extensions Table is defined in the following @value{LANGUAGE}
@value{STRUCTURE}:

@ifset is-C
@example
typedef User_extensions_routine           rtems_extension;
typedef User_extensions_thread_create_extension   
           rtems_task_create_extension;
typedef User_extensions_thread_delete_extension   
           rtems_task_delete_extension;
typedef User_extensions_thread_start_extension    
           rtems_task_start_extension;
typedef User_extensions_thread_restart_extension  
           rtems_task_restart_extension;
typedef User_extensions_thread_switch_extension   
           rtems_task_switch_extension;
typedef User_extensions_thread_begin_extension    
           rtems_task_begin_extension;
typedef User_extensions_thread_exitted_extension  
           rtems_task_exitted_extension;
typedef User_extensions_fatal_extension   rtems_fatal_extension;

typedef User_extensions_Table             rtems_extensions_table;

typedef struct @{
  rtems_task_create_extension      thread_create;
  rtems_task_start_extension       thread_start;
  rtems_task_restart_extension     thread_restart;
  rtems_task_delete_extension      thread_delete;
  rtems_task_switch_extension      thread_switch;
  rtems_task_begin_extension       thread_begin;
  rtems_task_exitted_extension     thread_exitted;
  rtems_fatal_extension            fatal;
@} User_extensions_Table;
@end example
@end ifset

@ifset is-Ada
@example
type Extensions_Table_Entry is
   record
      Thread_Create      : RTEMS.Thread_Create_Extension;
      Thread_Start       : RTEMS.Thread_Start_Extension;
      Thread_Restart     : RTEMS.Thread_Restart_Extension;
      Thread_Delete      : RTEMS.Thread_Delete_Extension;
      Thread_Switch      : RTEMS.Thread_Switch_Extension;
      Thread_Post_Switch : RTEMS.Thread_Post_Switch_Extension;
      Thread_Begin       : RTEMS.Thread_Begin_Extension;
      Thread_Exitted     : RTEMS.Thread_Exitted_Extension;
      Fatal            : RTEMS.Fatal_Error_Extension;
   end record;
@end example
@end ifset

@table @b

@item thread_create
is the address of the
user-supplied subroutine for the TASK_CREATE extension.  If this
extension for task creation is defined, it is called from the
task_create directive.  A value of NULL indicates that no
extension is provided.

@item thread_start
is the address of the user-supplied
subroutine for the TASK_START extension.  If this extension for
task initiation is defined, it is called from the task_start
directive.  A value of NULL indicates that no extension is
provided.

@item thread_restart
is the address of the user-supplied
subroutine for the TASK_RESTART extension.  If this extension
for task re-initiation is defined, it is called from the
task_restart directive.  A value of NULL indicates that no
extension is provided.

@item thread_delete
is the address of the user-supplied
subroutine for the TASK_DELETE extension.  If this RTEMS
extension for task deletion is defined, it is called from the
task_delete directive.  A value of NULL indicates that no
extension is provided.

@item thread_switch
is the address of the user-supplied
subroutine for the task context switch extension.  This
subroutine is called from RTEMS dispatcher after the current
task has been swapped out but before the new task has been
swapped in.  A value of NULL indicates that no extension is
provided.  As this routine is invoked after saving the current
task's context and before restoring the heir task's context, it
is not necessary for this routine to save and restore any
registers.

@item thread_begin
is the address of the user-supplied
subroutine which is invoked immediately before a task begins
execution.  It is invoked in the context of the beginning task.
A value of NULL indicates that no extension is provided.

@item thread_exitted
is the address of the user-supplied
subroutine which is invoked when a task exits.  This procedure
is responsible for some action which will allow the system to
continue execution (i.e. delete or restart the task) or to
terminate with a fatal error.  If this field is set to NULL, the
default RTEMS TASK_EXITTED handler will be invoked.

@item fatal
is the address of the user-supplied
subroutine for the FATAL extension.  This RTEMS extension of
fatal error handling is called from the 
@code{@value{DIRPREFIX}fatal_error_occurred}
directive.  If the user's fatal error handler returns or if this
entry is NULL then the default RTEMS fatal error handler will be
executed.

@end table

A typical declaration for a User Extension Table
which defines the TASK_CREATE, TASK_DELETE, TASK_SWITCH, and
FATAL extension might appear as follows:

@ifset is-C
@example
rtems_extensions_table User_extensions = @{
   task_create_extension,
   NULL,
   NULL,
   task_delete_extension,
   task_switch_extension,
   NULL,
   NULL,
   fatal_extension
@};
@end example
@end ifset

@ifset is-Ada
User_Extensions : RTEMS.Extensions_Table := (
   Task_Create_Extension'Access,
   null,
   null,
   Task_Delete_Extension'Access,
   Task_Switch_Extension'Access,
   null,
   null,
   Fatal_Extension'Access
);
@example

@end example
@end ifset

More information regarding the user extensions is
provided in the User Extensions chapter.

@section Multiprocessor Configuration Table

@cindex Multiprocessor Configuration Table

The Multiprocessor Configuration Table contains
information needed when using RTEMS in a multiprocessor
configuration.  Many of the details associated with configuring
a multiprocessor system are dependent on the multiprocessor
communications layer provided by the user.  The address of the
Multiprocessor Configuration Table should be placed in the
@code{User_multiprocessing_table} entry in the primary Configuration
Table.  Further details regarding many of the entries in the
Multiprocessor Configuration Table will be provided in the
Multiprocessing chapter.  


When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the macro @code{CONFIGURE_MP_APPLICATION} must
be defined to automatically generate the Multiprocessor Configuration Table.
If @code{CONFIGURE_MP_APPLICATION}, is not defined, then a NULL pointer
is configured as the address of this table.

The format of the Multiprocessor Configuration Table is defined in 
the following @value{LANGUAGE} @value{STRUCTURE}:

@ifset is-C
@example
typedef struct @{
  rtems_unsigned32  node;
  rtems_unsigned32  maximum_nodes;
  rtems_unsigned32  maximum_global_objects;
  rtems_unsigned32  maximum_proxies;
  rtems_mpci_table *User_mpci_table;
@} rtems_multiprocessing_table;
@end example
@end ifset

@ifset is-Ada
@example
type Multiprocessing_Table is
   record
      Node                   : RTEMS.Unsigned32;
      Maximum_Nodes          : RTEMS.Unsigned32;
      Maximum_Global_Objects : RTEMS.Unsigned32;
      Maximum_Proxies        : RTEMS.Unsigned32;
      User_MPCI_Table        : RTEMS.MPCI_Table_Pointer;
   end record;

type Multiprocessing_Table_Pointer is access all Multiprocessing_Table;
@end example
@end ifset

@table @b
@item node
is a unique processor identifier
and is used in routing messages between nodes in a
multiprocessor configuration.  Each processor must have a unique
node number.  RTEMS assumes that node numbers start at one and
increase sequentially.  This assumption can be used to advantage
by the user-supplied MPCI layer.  Typically, this requirement is
made when the node numbers are used to calculate the address of
inter-processor communication links.  Zero should be avoided as
a node number because some MPCI layers use node zero to
represent broadcasted packets.  Thus, it is recommended that
node numbers start at one and increase sequentially.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MP_NODE_NUMBER}.
If not defined by the application, then the @code{CONFIGURE_MP_NODE_NUMBER}
macro defaults to the value of the @code{NODE_NUMBER} macro which is
set on the compiler command line by the RTEMS Multiprocessing Test Suites.


@item maximum_nodes
is the number of processor nodes in the system.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MP_MAXIMUM_NODES}.
If not defined by the application, then the @code{CONFIGURE_MP_MAXIMUM_NODES}
macro defaults to the value 2.

@item maximum_global_objects
is the maximum number of global objects which can exist at any
given moment in the entire system.  If this parameter is not the
same on all nodes in the system, then a fatal error is generated
to inform the user that the system is inconsistent.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS}.
If not defined by the application, then the
@code{CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS} macro defaults to the value 32.


@item maximum_proxies
is the maximum number of proxies which can exist at any given moment
on this particular node.  A proxy is a substitute task control block
which represent a task residing on a remote node when that task blocks
on a remote object.  Proxies are used in situations in which delayed
interaction is required with a remote node.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MP_MAXIMUM_PROXIES}.
If not defined by the application, then the @code{CONFIGURE_MP_MAXIMUM_PROXIES}
macro defaults to the value 32.


@item User_mpci_table
is the address of the Multiprocessor Communications Interface
Table.  This table contains the entry points of user-provided functions
which constitute the multiprocessor communications layer.  This table
must be provided in multiprocessor configurations with all
entries configured.  The format of this table and details
regarding its entries can be found in the next section.
When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the value for this field corresponds
to the setting of the macro @code{CONFIGURE_MP_MPCI_TABLE_POINTER}.
If not defined by the application, then the
@code{CONFIGURE_MP_MPCI_TABLE_POINTER} macro defaults to the 
address of the table named @code{MPCI_table}.


@end table

@section Multiprocessor Communications Interface Table

@cindex  Multiprocessor Communications Interface Table

This table defines the set of callouts that must be provided by 
an Multiprocessor Communications Interface implementation.  

When using the @code{confdefs.h} mechanism for configuring
an RTEMS application, the name of this table is assumed
to be @code{MPCI_table} unless the application sets
the @code{CONFIGURE_MP_MPCI_TABLE_POINTER} when configuring a
multiprocessing system. 

The format of this table is defined in 
the following @value{LANGUAGE} @value{STRUCTURE}:

@ifset is-C
@example
typedef struct @{
  rtems_unsigned32                 default_timeout; /* in ticks */
  rtems_unsigned32                 maximum_packet_size;
  rtems_mpci_initialization_entry initialization;
  rtems_mpci_get_packet_entry     get_packet;
  rtems_mpci_return_packet_entry  return_packet;
  rtems_mpci_send_entry           send;
  rtems_mpci_receive_entry        receive;
@} rtems_mpci_table;
@end example
@end ifset

@ifset is-Ada
@example
type MPCI_Table is
   record
      Default_Timeout     : RTEMS.Unsigned32; -- in ticks
      Maximum_Packet_Size : RTEMS.Unsigned32;
      Initialization      : RTEMS.MPCI_Initialization_Entry;
      Get_Packet          : RTEMS.MPCI_Get_Packet_Entry;
      Return_Packet       : RTEMS.MPCI_Return_Packet_Entry;
      Send                : RTEMS.MPCI_Send_Entry;
      Receive             : RTEMS.MPCI_Receive_Entry;
   end record;

type MPCI_Table_Pointer is access all MPCI_Table;
@end example
@end ifset

@table @b
@item default_timeout
is the default maximum length of time a task should block waiting for
a response to a directive which results in communication with a remote node.
The maximum length of time is a function the user supplied
multiprocessor communications layer and the media used.  This
timeout only applies to directives which would not block if the
operation were performed locally.

@item maximum_packet_size
is the size in bytes of the longest packet which the MPCI layer is capable
of sending.  This value should represent the total number of bytes available
for a RTEMS interprocessor messages.

@item initialization
is the address of the entry point for the initialization procedure of the
user supplied multiprocessor communications layer.

@item get_packet
is the address of the entry point for the procedure called by RTEMS to
obtain a packet from the user supplied multiprocessor communications layer.

@item return_packet
is the address of the entry point for the procedure called by RTEMS to
return a packet to the user supplied multiprocessor communications layer.

@item send
is the address of the entry point for the procedure called by RTEMS to
send an envelope to another node.  This procedure is part of the user
supplied multiprocessor communications layer.

@item receive
is the address of the entry point for the
procedure called by RTEMS to retrieve an envelope containing a
message from another node.  This procedure is part of the user
supplied multiprocessor communications layer.

@end table

More information regarding the required functionality of these
entry points is provided in the Multiprocessor chapter.

@section Determining Memory Requirements

Since memory is a critical resource in many real-time
embedded systems, the RTEMS Classic API was specifically designed to allow
unused managers to be forcibly excluded from the run-time environment.
This allows the application designer the flexibility to tailor
RTEMS to most efficiently meet system requirements while still
satisfying even the most stringent memory constraints.  As
result, the size of the RTEMS executive is application
dependent.  A Memory Requirements worksheet is provided in the
Applications Supplement document for a specific target
processor.  This worksheet can be used to calculate the memory
requirements of a custom RTEMS run-time environment.  To insure
that enough memory is allocated for future versions of RTEMS,
the application designer should round these memory requirements
up.  The following Classic API managers may be optionally excluded:

@itemize @bullet
@item signal
@item region
@item dual ported memory
@item event
@item multiprocessing
@item partition
@item timer
@item semaphore
@item message
@item rate monotonic
@end itemize

RTEMS is designed to be built and installed as a library
that is linked into the application.  As such, much of
RTEMS is implemented in such a way that there is a single
entry point per source file.  This avoids having the 
linker being forced to pull large object files in their
entirety into an application when the application references
a single symbol.

RTEMS based applications must somehow provide memory
for RTEMS' code and data space.  Although RTEMS' data space must
be in RAM, its code space can be located in either ROM or RAM.
In addition, the user must allocate RAM for the RTEMS RAM
Workspace.  The size of this area is application dependent and
can be calculated using the formula provided in the Memory
Requirements chapter of the Applications Supplement document
for a specific target processor.

All private RTEMS data variables and routine names used by
RTEMS begin with the underscore ( _ ) character followed by an
upper-case letter.  If RTEMS is linked with an application, then
the application code should NOT contain any symbols which begin
with the underscore character and followed by an upper-case
letter to avoid any naming conflicts.  All RTEMS directive names
should be treated as reserved words.

@section Sizing the RTEMS RAM Workspace

The RTEMS RAM Workspace is a user-specified block of
memory reserved for use by RTEMS.  The application should NOT
modify this memory.  This area consists primarily of the RTEMS
data structures whose exact size depends upon the values
specified in the Configuration Table.  In addition, task stacks
and floating point context areas are dynamically allocated from
the RTEMS RAM Workspace.

The @code{confdefs.h} mechanism calcalutes the size
of the RTEMS RAM Workspace automatically.  It assumes that
all tasks are floating point and that all will be allocated
the miminum stack space.  This calculation also automatically
includes the memory that will be allocated for internal use
by RTEMS.  The following macros may be set
by the application to make the calculation
of memory required more accurate:

@itemize @bullet

@item @code{CONFIGURE_MEMORY_OVERHEAD}
@item @code{CONFIGURE_EXTRA_TASK_STACKS}

@end itemize

The starting address of the RTEMS RAM Workspace must
be aligned on a four-byte boundary.  Failure to properly align
the workspace area will result in the 
@code{@value{DIRPREFIX}fatal_error_occurred}
directive being invoked with the
@code{@value{RPREFIX}INVALID_ADDRESS} error code.

A worksheet is provided in the @b{Memory Requirements}
chapter of the Applications Supplement document for a specific
target processor to assist the user in calculating the minimum
size of the RTEMS RAM Workspace for each application.  The value
calculated with this worksheet is the minimum value that should
be specified as the @code{work_space_size} parameter of the
Configuration Table.  

The allocation of objects can operate in two modes. The default mode
has an object number ceiling. No more than the specified number of
objects can be allocated from the RTEMS RAM Workspace. The number of objects
specified in the particular API Configuration table fields are
allocated at initialisation. The second mode allows the number of
objects to grow to use the available free memory in the RTEMS RAM Workspace.

The auto-extending mode can be enabled individually for each object
type by using the macro @code{rtems_resource_unlimited}. This takes a value
as a parameter, and is used to set the object maximum number field in
an API Configuration table. The value is an allocation unit size. When
RTEMS is required to grow the object table it is grown by this
size. The kernel will return the object memory back to the RTEMS RAM Workspace
when an object is destroyed. The kernel will only return an allocated
block of objects to the RTEMS RAM Workspace if at least half the allocation
size of free objects remain allocated. RTEMS always keeps one
allocation block of objects allocated. Here is an example of using 
@code{rtems_resource_unlimited}:

@example
#define CONFIGURE_MAXIMUM_TASKS rtems_resource_unlimited(5)
@end example

The user is cautioned that future versions of RTEMS may not have the
same memory requirements per object. Although the value calculated is
suficient for a particular target processor and release of RTEMS,
memory usage is subject to change across versions and target
processors.  The user is advised to allocate somewhat more memory than
the worksheet recommends to insure compatibility with future releases
for a specific target processor and other target processors. To avoid
problems, the user should recalculate the memory requirements each
time one of the following events occurs:

@itemize @bullet
@item a configuration parameter is modified,
@item task or interrupt stack requirements change,
@item task floating point attribute is altered,
@item RTEMS is upgraded, or
@item the target processor is changed.
@end itemize

Failure to provide enough space in the RTEMS RAM
Workspace will result in the 
@code{@value{DIRPREFIX}fatal_error_occurred} directive
being invoked with the appropriate error code.
