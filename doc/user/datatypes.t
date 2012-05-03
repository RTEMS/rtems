@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter RTEMS Data Types

@section Introduction

This chapter contains a complete list of the RTEMS primitive
data types in alphabetical order.  This is intended to be
an overview and the user is encouraged to look at the appropriate
chapters in the manual for more information about the
usage of the various data types.

@section List of Data Types

The following is a complete list of the RTEMS primitive
data types in alphabetical order:

@itemize @bullet 

@findex rtems_address
@item @code{@value{DIRPREFIX}address} is the data type used to manage 
addresses.  It is equivalent to
@ifset is-C
a "void *" pointer.
@end ifset
@ifset is-Ada
the System.Address data type.
@end ifset

@findex rtems_asr
@item @code{@value{DIRPREFIX}asr} is the return type for an
RTEMS ASR.

@findex rtems_asr_entry
@item @code{@value{DIRPREFIX}asr_entry} is the address of
the entry point to an RTEMS ASR.

@findex rtems_attribute
@item @code{@value{DIRPREFIX}attribute} is the data type used
to manage the attributes for RTEMS objects.  It is primarily
used as an argument to object create routines to specify 
characteristics of the new object.

@findex rtems_boolean
@item @code{@value{DIRPREFIX}boolean} may only take on the
values of @code{TRUE} and @code{FALSE}.

This type is deprecated. Use "bool" instead.

@findex rtems_context
@item @code{@value{DIRPREFIX}context} is the CPU dependent
data structure used to manage the integer and system
register portion of each task's context.

@findex rtems_context_fp
@item @code{@value{DIRPREFIX}context_fp} is the CPU dependent
data structure used to manage the floating point portion of
each task's context.

@findex rtems_device_driver
@item @code{@value{DIRPREFIX}device_driver} is the
return type for a RTEMS device driver routine.

@findex rtems_device_driver_entry
@item @code{@value{DIRPREFIX}device_driver_entry} is the
entry point to a RTEMS device driver routine. 

@findex rtems_device_major_number
@item @code{@value{DIRPREFIX}device_major_number} is the 
data type used to manage device major numbers.

@findex rtems_device_minor_number
@item @code{@value{DIRPREFIX}device_minor_number} is the 
data type used to manage device minor numbers.

@findex rtems_double
@item @code{@value{DIRPREFIX}double} is the RTEMS data
type that corresponds to double precision floating point
on the target hardware. 

This type is deprecated. Use "double" instead.

@findex rtems_event_set
@item @code{@value{DIRPREFIX}event_set} is the data
type used to manage and manipulate RTEMS event sets
with the Event Manager.

@findex rtems_extension
@item @code{@value{DIRPREFIX}extension} is the return type
for RTEMS user extension routines.
  
@findex rtems_fatal_extension
@item @code{@value{DIRPREFIX}fatal_extension} is the 
entry point for a fatal error user extension handler routine.

@findex rtems_id
@item @code{@value{DIRPREFIX}id} is the data type used
to manage and manipulate RTEMS object IDs.

@findex rtems_interrupt_frame
@item @code{@value{DIRPREFIX}interrupt_frame} is the 
data structure that defines the format of the interrupt
stack frame as it appears to a user ISR.  This data
structure may not be defined on all ports.

@findex rtems_interrupt_level
@item @code{@value{DIRPREFIX}interrupt_level} is the 
data structure used with the @code{@value{DIRPREFIX}interrupt_disable},
@code{@value{DIRPREFIX}interrupt_enable}, and 
@code{@value{DIRPREFIX}interrupt_flash} routines.  This
data type is CPU dependent and usually corresponds to
the contents of the processor register containing
the interrupt mask level.

@findex rtems_interval
@item @code{@value{DIRPREFIX}interval} is the data
type used to manage and manipulate time intervals.
Intervals are non-negative integers used to measure
the length of time in clock ticks.

@findex rtems_isr
@item @code{@value{DIRPREFIX}isr} is the return type
of a function implementing an RTEMS ISR.

@findex rtems_isr_entry
@item @code{@value{DIRPREFIX}isr_entry} is the address of
the entry point to an RTEMS ISR.  It is equivalent to the
entry point of the function implementing the ISR.

@findex rtems_mp_packet_classes
@item @code{@value{DIRPREFIX}mp_packet_classes} is the 
enumerated type which specifies the categories of 
multiprocessing messages.  For example, one of the
classes is for messages that must be processed by
the Task Manager.

@findex rtems_mode
@item @code{@value{DIRPREFIX}mode} is the data type
used to manage and dynamically manipulate the execution
mode of an RTEMS task.

@findex rtems_mpci_entry
@item @code{@value{DIRPREFIX}mpci_entry} is the return type
of an RTEMS MPCI routine.

@findex rtems_mpci_get_packet_entry
@item @code{@value{DIRPREFIX}mpci_get_packet_entry} is the address of
the entry point to the get packet routine for an MPCI implementation.

@findex rtems_mpci_initialization_entry
@item @code{@value{DIRPREFIX}mpci_initialization_entry} is the address of
the entry point to the initialization routine for an MPCI implementation.

@findex rtems_mpci_receive_packet_entry
@item @code{@value{DIRPREFIX}mpci_receive_packet_entry} is the address of
the entry point to the receive packet routine for an MPCI implementation.

@findex rtems_mpci_return_packet_entry
@item @code{@value{DIRPREFIX}mpci_return_packet_entry} is the address of
the entry point to the return packet routine for an MPCI implementation.

@findex rtems_mpci_send_packet_entry
@item @code{@value{DIRPREFIX}mpci_send_packet_entry} is the address of
the entry point to the send packet routine for an MPCI implementation.

@findex rtems_mpci_table
@item @code{@value{DIRPREFIX}mpci_table} is the data structure
containing the configuration information for an MPCI.

@findex rtems_name
@item @code{@value{DIRPREFIX}name} is the data type used to
contain the name of a Classic API object.  It is an unsigned 
thirty-two bit integer which can be treated as a numeric
value or initialized using @code{@value{DIRPREFIX}build_name} to
contain four ASCII characters.

@findex rtems_option
@item @code{@value{DIRPREFIX}option} is the data type
used to specify which behavioral options the caller desires.
It is commonly used with potentially blocking directives to specify 
whether the caller is willing to block or return immediately with an error
indicating that the resource was not available.

@findex rtems_packet_prefix
@item @code{@value{DIRPREFIX}packet_prefix} is the data structure
that defines the first bytes in every packet sent between nodes
in an RTEMS multiprocessor system.  It contains routing information
that is expected to be used by the MPCI layer.

@findex rtems_signal_set
@item @code{@value{DIRPREFIX}signal_set} is the data
type used to manage and manipulate RTEMS signal sets
with the Signal Manager.

@findex int8_t
@item @code{int8_t} is the C99 data type that corresponds to signed eight
bit integers.  This data type is defined by RTEMS in a manner that
ensures it is portable across different target processors.

@findex int16_t
@item @code{int16_t} is the C99 data type that corresponds to signed
sixteen bit integers.  This data type is defined by RTEMS in a manner
that ensures it is portable across different target processors.

@findex int32_t
@item @code{int32_t} is the C99 data type that corresponds to signed
thirty-two bit integers.  This data type is defined by RTEMS in a manner
that ensures it is portable across different target processors.

@findex int64_t
@item @code{int64_t} is the C99 data type that corresponds to signed
sixty-four bit integers.  This data type is defined by RTEMS in a manner
that ensures it is portable across different target processors.

@findex rtems_single
@item @code{@value{DIRPREFIX}single} is the RTEMS data
type that corresponds to single precision floating point
on the target hardware. 

This type is deprecated. Use "float" instead.

@findex rtems_status_codes
@item @code{@value{DIRPREFIX}status_codes} is the return type for most
RTEMS services.  This is an enumerated type of approximately twenty-five
values.  In general, when a service returns a particular status code, it
indicates that a very specific error condition has occurred.

@findex rtems_task
@item @code{@value{DIRPREFIX}task} is the return type for an
RTEMS Task.

@findex rtems_task_argument
@item @code{@value{DIRPREFIX}task_argument} is the data
type for the argument passed to each RTEMS task. In RTEMS 4.7
and older, this is an unsigned thirty-two bit integer.  In
RTEMS 4.8 and newer, this is based upon the C99 type @code{uintptr_t}
which is guaranteed to be an integer large enough to hold a 
pointer on the target architecture.

@findex rtems_task_begin_extension
@item @code{@value{DIRPREFIX}task_begin_extension} is the 
entry point for a task beginning execution user extension handler routine.

@findex rtems_task_create_extension
@item @code{@value{DIRPREFIX}task_create_extension} is the 
entry point for a task creation execution user extension handler routine.

@findex rtems_task_delete_extension
@item @code{@value{DIRPREFIX}task_delete_extension} is the 
entry point for a task deletion user extension handler routine.

@findex rtems_task_entry
@item @code{@value{DIRPREFIX}task_entry} is the address of
the entry point to an RTEMS ASR.  It is equivalent to the
entry point of the function implementing the ASR.

@findex rtems_task_exitted_extension
@item @code{@value{DIRPREFIX}task_exitted_extension} is the 
entry point for a task exitted user extension handler routine.

@findex rtems_task_priority
@item @code{@value{DIRPREFIX}task_priority} is the data type
used to manage and manipulate task priorities.

@findex rtems_task_restart_extension
@item @code{@value{DIRPREFIX}task_restart_extension} is the 
entry point for a task restart user extension handler routine.

@findex rtems_task_start_extension
@item @code{@value{DIRPREFIX}task_start_extension} is the 
entry point for a task start user extension handler routine.

@findex rtems_task_switch_extension
@item @code{@value{DIRPREFIX}task_switch_extension} is the 
entry point for a task context switch user extension handler routine.

@findex rtems_tcb
@item @code{@value{DIRPREFIX}tcb} is the data structure associated
with each task in an RTEMS system.

@findex rtems_time_of_day
@item @code{@value{DIRPREFIX}time_of_day} is the data structure
used to manage and manipulate calendar time in RTEMS.

@findex rtems_timer_service_routine
@item @code{@value{DIRPREFIX}timer_service_routine} is the 
return type for an RTEMS Timer Service Routine.

@findex rtems_timer_service_routine_entry
@item @code{@value{DIRPREFIX}timer_service_routine_entry} is the address of
the entry point to an RTEMS TSR.  It is equivalent to the
entry point of the function implementing the TSR.

@findex rtems_vector_number
@item @code{@value{DIRPREFIX}vector_number} is the data
type used to manage and manipulate interrupt vector numbers.

@findex uint8_t
@item @code{uint8_t} is the C99 data type that corresponds to unsigned
eight bit integers.  This data type is defined by RTEMS in a manner that
ensures it is portable across different target processors.

@findex uint16_t
@item @code{uint16_t} is the C99 data type that corresponds to unsigned
sixteen bit integers.  This data type is defined by RTEMS in a manner
that ensures it is portable across different target processors.

@findex uint32_t
@item @code{uint32_t} is the C99 data type that corresponds to unsigned
thirty-two bit integers.  This data type is defined by RTEMS in a manner
that ensures it is portable across different target processors.

@findex uint64_t
@item @code{uint64_t} is the C99 data type that corresponds to unsigned
sixty-four bit integers.  This data type is defined by RTEMS in a manner
that ensures it is portable across different target processors.

@findex uintptr_t
@item @code{uintptr_t} is the C99 data type that corresponds to the
unsigned integer type that is of sufficient size to represent addresses
as unsigned integers.  This data type is defined by RTEMS in a manner
that ensures it is portable across different target processors.

@end itemize
