@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@node Configuring a System, Configuring a System Configuration Table, EXTENSION_DELETE - Delete a extension set, Top
@end ifinfo
@chapter Configuring a System
@ifinfo
@menu
* Configuring a System Configuration Table::
* Configuring a System RTEMS API Configuration Table::
* Configuring a System POSIX API Configuration Table::
* Configuring a System CPU Dependent Information Table::
* Configuring a System Initialization Task Table::
* Configuring a System Driver Address Table::
* Configuring a System User Extensions Table::
* Configuring a System Multiprocessor Configuration Table::
* Configuring a System Multiprocessor Communications Interface Table::
* Configuring a System Determining Memory Requirements::
* Configuring a System Sizing the RTEMS RAM Workspace::
@end menu
@end ifinfo

@ifinfo
@node Configuring a System Configuration Table, Configuring a System RTEMS API Configuration Table, Configuring a System, Configuring a System
@end ifinfo
@section Configuration Table

The RTEMS Configuration Table is used to tailor an
application for its specific needs.  For example, the user can
configure the number of device drivers or which APIs may be used.
THe address of the user-defined Configuration Table is passed as an
argument to the initialize_executive directive, which MUST be
the first RTEMS directive called.  The RTEMS Configuration Table
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
       Work_Space_Start           : RTEMS.Address;
       Work_Space_Size            : RTEMS.Unsigned32;
       Maximum_Extensions         : RTEMS.Unsigned32;
       Microseconds_Per_Tick      : RTEMS.Unsigned32;
       Ticks_Per_Timeslice        : RTEMS.Unsigned32;
       Maximum_Devices            : RTEMS.Unsigned32;
       Number_Of_Device_Drivers   : RTEMS.Unsigned32;
       Device_Driver_Table        : RTEMS.Driver_Address_Table_Pointer;
       User_Extension_Table       : RTEMS.Extensions_Table_Pointer;
       User_Multiprocessing_Table : RTEMS.Multiprocessing_Table_Pointer;
       RTEMS_API_Configuration    : RTEMS.API_Configuration_Table_Pointer;
       POSIX_API_Configuration    :
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
initialize_executive.

@item work_space_size
is the calculated size of the
RTEMS RAM Workspace.  The section Sizing the RTEMS RAM Workspace
details how to arrive at this number.

@item microseconds_per_tick
is number of microseconds per clock tick.

@item ticks_per_timeslice
is the number of clock ticks for a timeslice.

@item maximum_devices
is the maximum number of devices that can be registered.

@item number_of_device_drivers
is the number of device drivers for the system.  There should be
the same number of entries in the Device Driver Table.  If this field
is zero, then the User_driver_address_table entry should be NULL.

@item Device_driver_table
is the address of the Device Driver Table.  This table contains the entry
points for each device driver.  If the number_of_device_drivers field is zero,
then this entry should be NULL. The format of this table will be
discussed below.

@item User_extension_table
is the address of the User
Extension Table.  This table contains the entry points for the
static set of optional user extensions.  If no user extensions
are configured, then this entry should be NULL.  The format of
this table will be discussed below.

@item User_multiprocessing_table
is the address of the Multiprocessor Configuration Table.  This
table contains information needed by RTEMS only when used in a multiprocessor
configuration.  This field must be NULL when RTEMS is used in a
single processor configuration.

@item RTEMS_api_configuration
is the address of the RTEMS API Configuration Table.  This table
contains information needed by the RTEMS API.  This field should be
NULL if the RTEMS API is not used.  [NOTE: Currently the RTEMS API
is required to support support components such as BSPs and libraries 
which use this API.]

@item POSIX_api_configuration
is the address of the POSIX API Configuration Table.  This table
contains information needed by the POSIX API.  This field should be
NULL if the POSIX API is not used.

@end table

@ifinfo
@node Configuring a System RTEMS API Configuration Table, Configuring a System POSIX API Configuration Table, Configuring a System Configuration Table, Configuring a System
@end ifinfo
@section RTEMS API Configuration Table

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

@item maximum_timers
is the maximum number of timers
that can be concurrently active in the system.

@item maximum_semaphores
is the maximum number of
semaphores that can be concurrently active in the system.

@item maximum_message_queues
is the maximum number of
message queues that can be concurrently active in the system.

@item maximum_partitions
is the maximum number of
partitions that can be concurrently active in the system.

@item maximum_regions
is the maximum number of regions
that can be concurrently active in the system.

@item maximum_ports
is the maximum number of ports into
dual-port memory areas that can be concurrently active in the
system.

@item number_of_initialization_tasks
is the number of initialization tasks configured.  At least one
initialization task must be configured.

@item User_initialization_tasks_table
is the address of the Initialization Task Table. This table contains the
information needed to create and start each of the
initialization tasks.  The format of this table will be discussed below.

@end table

@ifinfo
@node Configuring a System POSIX API Configuration Table, Configuring a System CPU Dependent Information Table, Configuring a System RTEMS API Configuration Table, Configuring a System
@end ifinfo
@section POSIX API Configuration Table

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

@item maximum_mutexes
is the maximum number of mutexes that can be concurrently 
active in the system.

@item maximum_condition_variables
is the maximum number of condition variables that can be 
concurrently active in the system.

@item maximum_keys
is the maximum number of keys that can be concurrently active in the system.

@item maximum_queued_signals
is the maximum number of queued signals that can be concurrently 
pending in the system.

@item number_of_initialization_threads
is the number of initialization threads configured.  At least one
initialization threads must be configured.
 
@item User_initialization_threads_table
is the address of the Initialization Threads Table. This table contains the
information needed to create and start each of the initialization threads.  
The format of each entry in this table is defined in the 
posix_initialization_threads_table @value{STRUCTURE}.

@end table

@ifinfo
@node Configuring a System CPU Dependent Information Table, Configuring a System Initialization Task Table, Configuring a System POSIX API Configuration Table, Configuring a System
@end ifinfo
@section CPU Dependent Information Table

The CPU Dependent Information Table is used to
describe processor dependent information required by RTEMS.
This table is generally used to supply RTEMS with information
only known by the Board Support Package.  The contents of this
table are discussed in the CPU Dependent Information Table
chapter of the Applications Supplement document for a specific
target processor.

@ifinfo
@node Configuring a System Initialization Task Table, Configuring a System Driver Address Table, Configuring a System CPU Dependent Information Table, Configuring a System
@end ifinfo
@section Initialization Task Table

The Initialization Task Table is used to describe
each of the user initialization tasks to the Initialization
Manager.  The table contains one entry for each initialization
task the user wishes to create and start.  The fields of this
data structure directly correspond to arguments to the
task_create and task_start directives.  The number of entries is
found in the number_of_initialization_tasks entry in the
Configuration Table.  The format of each entry in the
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

@ifinfo
@node Configuring a System Driver Address Table, Configuring a System User Extensions Table, Configuring a System Initialization Task Table, Configuring a System
@end ifinfo
@section Driver Address Table

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
is the address of the entry point called by io_initialize
to initialize a device driver and its associated devices.

@item open
is the address of the entry point called by io_open.

@item close
is the address of the entry point called by io_close.

@item read
is the address of the entry point called by io_read.

@item write
is the address of the entry point called by io_write.

@item control
is the address of the entry point called by io_control.

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

@ifinfo
@node Configuring a System User Extensions Table, Configuring a System Multiprocessor Configuration Table, Configuring a System Driver Address Table, Configuring a System
@end ifinfo
@section User Extensions Table

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
typedef User_extensions_routine                   rtems_extension;
typedef User_extensions_thread_create_extension   rtems_task_create_extension;
typedef User_extensions_thread_delete_extension   rtems_task_delete_extension;
typedef User_extensions_thread_start_extension    rtems_task_start_extension;
typedef User_extensions_thread_restart_extension  rtems_task_restart_extension;
typedef User_extensions_thread_switch_extension   rtems_task_switch_extension;
typedef User_extensions_thread_begin_extension    rtems_task_begin_extension;
typedef User_extensions_thread_exitted_extension  rtems_task_exitted_extension;
typedef User_extensions_fatal_extension           rtems_fatal_extension;

typedef User_extensions_Table                     rtems_extensions_table;

typedef struct @{
  rtems_task_create_extension      thread_create;
  rtems_task_start_extension       thread_start;
  rtems_task_restart_extension     thread_restart;
  rtems_task_delete_extension      thread_delete;
  rtems_task_switch_extension      thread_switch;
  rtems_task_post_switch_extension thread_post_switch;
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

@item thread_post_switch
is the address of the
user-supplied subroutine for the post task context switch
extension.  This subroutine is called from RTEMS dispatcher in
the context of the task which has just been swapped in.

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
default RTEMS task_exitted handler will be invoked.

@item fatal
is the address of the user-supplied
subroutine for the FATAL extension.  This RTEMS extension of
fatal error handling is called from the fatal_error_occurred
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

@ifinfo
@node Configuring a System Multiprocessor Configuration Table, Configuring a System Multiprocessor Communications Interface Table, Configuring a System User Extensions Table, Configuring a System
@end ifinfo
@section Multiprocessor Configuration Table

The Multiprocessor Configuration Table contains
information needed when using RTEMS in a multiprocessor
configuration.  Many of the details associated with configuring
a multiprocessor system are dependent on the multiprocessor
communications layer provided by the user.  The address of the
Multiprocessor Configuration Table should be placed in the
User_multiprocessing_table entry in the primary Configuration
Table.  Further details regarding many of the entries in the
Multiprocessor Configuration Table will be provided in the
Multiprocessing chapter.  The format of the Multiprocessor
Configuration Table is defined in 
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

@item maximum_nodes
is the number of processor nodes in the system.

@item maximum_global_objects
is the maximum number of global objects which can exist at any
given moment in the entire system.  If this parameter is not the
same on all nodes in the system, then a fatal error is generated
to inform the user that the system is inconsistent.

@item maximum_proxies
is the maximum number of proxies which can exist at any given moment
on this particular node.  A proxy is a substitute task control block
which represent a task residing on a remote node when that task blocks
on a remote object.  Proxies are used in situations in which delayed
interaction is required with a remote node.

@item User_mpci_table
is the address of the Multiprocessor Communications Interface
Table.  This table contains the entry points of user-provided functions
which constitute the multiprocessor communications layer.  This table
must be provided in multiprocessor configurations with all
entries configured.  The format of this table and details
regarding its entries can be found in the next section.

@end table

@ifinfo
@node Configuring a System Multiprocessor Communications Interface Table, Configuring a System Determining Memory Requirements, Configuring a System Multiprocessor Configuration Table, Configuring a System
@end ifinfo
@section Multiprocessor Communications Interface Table

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

@ifinfo
@node Configuring a System Determining Memory Requirements, Configuring a System Sizing the RTEMS RAM Workspace, Configuring a System Multiprocessor Communications Interface Table, Configuring a System
@end ifinfo
@section Determining Memory Requirements

Since memory is a critical resource in many real-time
embedded systems, RTEMS was specifically designed to allow
unused managers to be excluded from the run-time environment.
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
up.  The following managers may be optionally excluded:

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

RTEMS based applications must somehow provide memory
for RTEMS' code and data space.  Although RTEMS' data space must
be in RAM, its code space can be located in either ROM or RAM.
In addition, the user must allocate RAM for the RTEMS RAM
Workspace.  The size of this area is application dependent and
can be calculated using the formula provided in the Memory
Requirements chapter of the Applications Supplement document
for a specific target processor.

All RTEMS data variables and routine names used by
RTEMS begin with the underscore ( _ ) character followed by an
upper-case letter.  If RTEMS is linked with an application, then
the application code should NOT contain any symbols which begin
with the underscore character and followed by an upper-case
letter to avoid any naming conflicts.  All RTEMS directive names
should be treated as reserved words.

@ifinfo
@node Configuring a System Sizing the RTEMS RAM Workspace, Multiprocessing Manager, Configuring a System Determining Memory Requirements, Configuring a System
@end ifinfo
@section Sizing the RTEMS RAM Workspace

The RTEMS RAM Workspace is a user-specified block of
memory reserved for use by RTEMS.  The application should NOT
modify this memory.  This area consists primarily of the RTEMS
data structures whose exact size depends upon the values
specified in the Configuration Table.  In addition, task stacks
and floating point context areas are dynamically allocated from
the RTEMS RAM Workspace.

The starting address of the RTEMS RAM Workspace must
be aligned on a four-byte boundary.  Failure to properly align
the workspace area will result in the fatal_error_occurred
directive being invoked with the @code{@value{RPREFIX}INVALID_ADDRESS} error code.

A worksheet is provided in the Memory Requirements
chapter of the Applications Supplement document for a specific
target processor to assist the user in calculating the minimum
size of the RTEMS RAM Workspace for each application.  The value
calculated with this worksheet is the minimum value that should
be specified as the work_space_size parameter of the
Configuration Table.  The user is cautioned that future versions
of RTEMS may not have the same memory requirements per object.
Although the value calculated is sufficient for a particular
target processor and release of RTEMS, memory usage is subject
to change across versions and target processors.  The user is
advised to allocate somewhat more memory than the worksheet
recommends to insure compatibility with future releases for a
specific target processor and other target processors.  To avoid
problems, the user should recalculate the memory requirements
each time one of the following events occurs:

@itemize @bullet
@item a configuration parameter is modified,
@item task or interrupt stack requirements change,
@item task floating point attribute is altered,
@item RTEMS is upgraded, or
@item the target processor is changed.
@end itemize

Failure to provide enough space in the RTEMS RAM
Workspace will result in the fatal_error_occurred directive
being invoked with the appropriate error code.
