@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@c The following macros from confdefs.h have not been discussed in this
@c chapter:
@c 
@c CONFIGURE_NEWLIB_EXTENSION
@c CONFIGURE_MALLOC_REGION
@c CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS
@c CONFIGURE_LIBIO_SEMAPHORES
@c CONFIGURE_INIT
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

@section Automatic Generation of System Configuration

@cindex confdefs.h
@fnindex confdefs.h

RTEMS provides the @code{confdefs.h} C language header file that
based on the setting of a variety of macros can automatically
produce nearly all of the configuration tables required
by an RTEMS application.  Rather than building the individual
tables by hand. the application simply specifies the values
for the configuration parameters it wishes to set.  In the following
example, the configuration information for a simple system with
a message queue and a time slice of 50 milliseconds is configured:

@example
@group
#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK   1000 /* 1 millisecond */
#define CONFIGURE_TICKS_PER_TIMESLICE       50 /* 50 milliseconds */

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
@end group
@end example

This system will begin execution with the single initialization task
named @code{Init}.  It will be configured to have both a console
device driver (for standard I/O) and a clock tick device driver.

For each configuration parameter in the configuration tables, the
macro corresponding to that field is discussed.  Most systems
can be easily configured using the @code{confdefs.h} mechanism.

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
       Device_Driver_Table          : RTEMS.Driver_Address_Table_Pointer;
       Number_Of_Initial_Extensions : RTEMS.Unsigned32;
       User_Extension_Table         : RTEMS.Extensions_Table_Pointer;
       User_Multiprocessing_Table   : RTEMS.Multiprocessing_Table_Pointer;
       RTEMS_API_Configuration      : RTEMS.API_Configuration_Table_Pointer;
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
to the setting of the macro @code{CONFIGURE_EXECUTIVE_RAM_WORK_AREA}.

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
If not defined by the application, then the @code{CONFIGURE_MAXIMUM_TASKS}
macro defaults to 10.
XXX

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

@item @code{CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER}
@item @code{CONFIGURE_TEST_NEEDS_CLOCK_DRIVER}
@item @code{CONFIGURE_TEST_NEEDS_TIMER_DRIVER}
@item @code{CONFIGURE_TEST_NEEDS_RTC_DRIVER}
@item @code{CONFIGURE_TEST_NEEDS_STUB_DRIVER}

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
is automatically generated when the @code{CONFIGURE_MPTEST}
is defined.  If @code{CONFIGURE_MPTEST} is not defined, the this
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

type API_Configuration_Table_Pointer is access all API_Configuration_Table;
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

   type POSIX_API_Configuration_Table is array ( RTEMS.Unsigned32 range <> ) of
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

type Initialization_Tasks_Table is array ( RTEMS.Unsigned32 range <> ) of
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
Initialization_Tasks : aliased RTEMS.Initialization_Tasks_Table( 1 .. 2 ) := (
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
an RTEMS application, the macro @code{CONFIGURE_MPTEST} must
be defined to automatically generate the Multiprocessor Configuration Table.
If @code{CONFIGURE_MPTEST}, is not defined, then a NULL pointer
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

CONFIGURE_MEMORY_OVERHEAD
CONFIGURE_EXTRA_TASK_STACKS

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
