c
@c  COPYRIGHT (c) 1988-2000.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter RTEMS Data Types

This chapter contains a complete list of the RTEMS primitive
data types in alphabetical order.  This is intended to be
an overview and the user is encouraged to look at the appropriate
chapters in the manual for more information about the
usage of the various data types.

@item @code{@value{DIRPREFIX}address} is the data type used to manage 
addresses.  It is equivalent to
@ifset is-C
a "void *" pointer.
@endif
@ifset is-Ada
the System.Address data type.
@endif

@item @code{@value{DIRPREFIX}asr} is the return type for an
RTEMS ASR.

@item @code{@value{DIRPREFIX}asr_entry} is the address of
the entry point to an RTEMS ASR.

@item @code{@value{DIRPREFIX}attribute} is the data type used
to manage the attributes for RTEMS objects.  It is primarily
used as an argument to object create routines.

@item @code{@value{DIRPREFIX}boolean} may only take on the
values of @code{TRUE} and @code{FALSE}.

@item @code{@value{DIRPREFIX}context} is the CPU dependent
data structure used to manage the integer and system
register portion of each task's context.

@item @code{@value{DIRPREFIX}context_fp} is the CPU dependent
data structure used to manage the floating point portion of
each task's context.

@item @code{@value{DIRPREFIX}device_driver} is the
return type for a RTEMS device driver routine.

@item @code{@value{DIRPREFIX}device_driver_entry} is the
entry point to a RTEMS device driver routine. 

@item @code{@value{DIRPREFIX}device_major_number} is the 
data type used to manage device major numbers.

@item @code{@value{DIRPREFIX}device_minor_number} is the 
data type used to manage device minor numbers.

@item @code{@value{DIRPREFIX}double} is the RTEMS data
type that corresponds to double precision floating point
on the target hardware. 

@item @code{@value{DIRPREFIX}event_set} is the data
type used to manage and manipulate RTEMS event sets
with the Event Manager.

@item @code{@value{DIRPREFIX}extension} is the return type
for RTEMS user extension routines.
  
@item @code{@value{DIRPREFIX}fatal_extension} is the 
entry point for a fatal error user extension handler routine.

@item @code{@value{DIRPREFIX}id} is the data type used
to manage and manipulate RTEMS object IDs.

@item @code{@value{DIRPREFIX}interrupt_frame} is the 
data structure that defines the format of the interrupt
stack frame as it appears to a user ISR.  This data
structure may not be defined on all ports.

@item @code{@value{DIRPREFIX}interrupt_level} is the 
data structure used with the @code{@value{DIRPREFIX}interrupt_disable},
@code{@value{DIRPREFIX}interrupt_enable}, and 
@code{@value{DIRPREFIX}interrupt_flash} routines.  This
data type is CPU dependent and usually corresponds to
the contents of the processor register containing
the interrupt mask level.

@item @code{@value{DIRPREFIX}interval} is the data
type used to manage and manipulate time intervals.

@item @code{@value{DIRPREFIX}isr} is the return type
of a function implementing an RTEMS ISR.

@item @code{@value{DIRPREFIX}isr_entry} is the address of
the entry point to an RTEMS ISR.  It is equivalent to the
entry point of the function implementing the ISR.

@item @code{@value{DIRPREFIX}mp_packet_classes} is the 
@item @code{@value{DIRPREFIX}mpci_entry} is the return type
of an RTEMS MPCI routine.

@item @code{@value{DIRPREFIX}mpci_get_packet_entry} is the address of
the entry point to the get packet routine for an MPCI implementation.

@item @code{@value{DIRPREFIX}mpci_initialization_entry} is the address of
the entry point to the initialization routine for an MPCI implementation.

@item @code{@value{DIRPREFIX}mpci_receive_packet_entry} is the address of
the entry point to the receive packet routine for an MPCI implementation.

@item @code{@value{DIRPREFIX}mpci_return_packet_entry} is the address of
the entry point to the return packet routine for an MPCI implementation.

@item @code{@value{DIRPREFIX}mpci_send_packet_entry} is the address of
the entry point to the send packet routine for an MPCI implementation.

@item @code{@value{DIRPREFIX}mpci_table} is the data structure
containing the configuration information for an MPCI.

@item @code{@value{DIRPREFIX}option} is the data type
used to specify which behavioral options the caller desires.
It is commonly used with potentially blocking directives to specify 
whether the caller is willing to block or return immediately with an error
indicating that the resource was not available.

@item @code{@value{DIRPREFIX}packet_prefix} is the data structure
that defines the first bytes in every packet sent between nodes
in an RTEMS multiprocessor system.  It contains routing information
that is expected to be used by the MPCI layer.

@item @code{@value{DIRPREFIX}signal_set} is the data
type used to manage and manipulate RTEMS signal sets
with the Signal Manager.

@item @code{@value{DIRPREFIX}signed8} is the data
type that corresponds to signed eight bit integers.
This data type is defined by RTEMS in a manner that
ensures it is portable across different target processors.

@item @code{@value{DIRPREFIX}signed16} is the data
type that corresponds to signed sixteen bit integers.
This data type is defined by RTEMS in a manner that
ensures it is portable across different target processors.

@item @code{@value{DIRPREFIX}signed32} is the data
type that corresponds to signed thirty-two bit integers.
This data type is defined by RTEMS in a manner that
ensures it is portable across different target processors.

@item @code{@value{DIRPREFIX}signed64} is the data
type that corresponds to signed sixty-four bit integers.
This data type is defined by RTEMS in a manner that
ensures it is portable across different target processors.

@item @code{@value{DIRPREFIX}single} is the RTEMS data
type that corresponds to single precision floating point
on the target hardware. 

@item @code{@value{DIRPREFIX}status_codes} is the 
@item @code{@value{DIRPREFIX}task} is the return type for an
RTEMS Task.

@item @code{@value{DIRPREFIX}task_argument} is the data
type for the argument passed to each RTEMS task. 

@item @code{@value{DIRPREFIX}task_begin_extension} is the 
entry point for a task beginning execution user extension handler routine.

@item @code{@value{DIRPREFIX}task_create_extension} is the 
entry point for a task creation execution user extension handler routine.

@item @code{@value{DIRPREFIX}task_delete_extension} is the 
entry point for a task deletion user extension handler routine.

@item @code{@value{DIRPREFIX}task_entry} is the address of
the entry point to an RTEMS ASR.  It is equivalent to the
entry point of the function implementing the ASR.

@item @code{@value{DIRPREFIX}task_exitted_extension} is the 
entry point for a task exitted user extension handler routine.

@item @code{@value{DIRPREFIX}task_priority} is the data type
used to manage and manipulate task priorities.

@item @code{@value{DIRPREFIX}task_restart_extension} is the 
entry point for a task restart user extension handler routine.

@item @code{@value{DIRPREFIX}task_start_extension} is the 
entry point for a task start user extension handler routine.

@item @code{@value{DIRPREFIX}task_switch_extension} is the 
entry point for a task context switch user extension handler routine.

@item @code{@value{DIRPREFIX}tcb} is the data structure associated
with each task in an RTEMS system.

@item @code{@value{DIRPREFIX}time_of_day} is the data structure
used to manage and manipulate calendar time in RTEMS.

@item @code{@value{DIRPREFIX}timer_service_routine} is the 
return type for an RTEMS Timer Service Routine.

@item @code{@value{DIRPREFIX}timer_service_routine_entry} is the address of
the entry point to an RTEMS TSR.  It is equivalent to the
entry point of the function implementing the TSR.

@item @code{@value{DIRPREFIX}unsigned8} is the data
type that corresponds to unsigned eight bit integers.
This data type is defined by RTEMS in a manner that
ensures it is portable across different target processors.

@item @code{@value{DIRPREFIX}unsigned16} is the data
type that corresponds to unsigned sixteen bit integers.
This data type is defined by RTEMS in a manner that
ensures it is portable across different target processors.

@item @code{@value{DIRPREFIX}unsigned32} is the data
type that corresponds to unsigned thirty-two bit integers.
This data type is defined by RTEMS in a manner that
ensures it is portable across different target processors.

@item @code{@value{DIRPREFIX}unsigned64} is the data
type that corresponds to unsigned sixty-four bit integers.
This data type is defined by RTEMS in a manner that
ensures it is portable across different target processors.

@item @code{@value{DIRPREFIX}vector_number} is the data
type used to manage and manipulate interrupt vector numbers.
