@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Message Manager

@cindex messages
@cindex message queues

@section Introduction

The message manager provides communication and
synchronization capabilities using RTEMS message queues.  The
directives provided by the message manager are:

@itemize @bullet
@item @code{@value{DIRPREFIX}message_queue_create} - Create a queue
@item @code{@value{DIRPREFIX}message_queue_ident} - Get ID of a queue
@item @code{@value{DIRPREFIX}message_queue_delete} - Delete a queue
@item @code{@value{DIRPREFIX}message_queue_send} - Put message at rear of a queue
@item @code{@value{DIRPREFIX}message_queue_urgent} - Put message at front of a queue
@item @code{@value{DIRPREFIX}message_queue_broadcast} - Broadcast N messages to a queue
@item @code{@value{DIRPREFIX}message_queue_receive} - Receive message from a queue
@item @code{@value{DIRPREFIX}message_queue_get_number_pending} - Get number of messages pending on a queue
@item @code{@value{DIRPREFIX}message_queue_flush} - Flush all messages on a queue
@end itemize

@section Background

@subsection Messages

A message is a variable length buffer where
information can be stored to support communication.  The length
of the message and the information stored in that message are
user-defined and can be actual data, pointer(s), or empty.

@subsection Message Queues

A message queue permits the passing of messages among
tasks and ISRs.  Message queues can contain a variable number of
messages.  Normally messages are sent to and received from the
queue in FIFO order using the @code{@value{DIRPREFIX}message_queue_send}
directive.  However, the @code{@value{DIRPREFIX}message_queue_urgent}
directive can be used to place
messages at the head of a queue in LIFO order.

Synchronization can be accomplished when a task can
wait for a message to arrive at a queue.  Also, a task may poll
a queue for the arrival of a message.

The maximum length message which can be sent is set
on a per message queue basis.

@subsection Building a Message Queue Attribute Set

@cindex message queue attributes

In general, an attribute set is built by a bitwise OR
of the desired attribute components.  The set of valid message
queue attributes is provided in the following table:

@itemize @bullet
@item @code{@value{RPREFIX}FIFO} - tasks wait by FIFO (default)
@item @code{@value{RPREFIX}PRIORITY} - tasks wait by priority
@item @code{@value{RPREFIX}LOCAL} - local message queue (default)
@item @code{@value{RPREFIX}GLOBAL} - global message queue
@end itemize


An attribute listed as a default is not required to
appear in the attribute list, although it is a good programming
practice to specify default attributes.  If all defaults are
desired, the attribute @code{@value{RPREFIX}DEFAULT_ATTRIBUTES}
should be specified on this call.

This example demonstrates the attribute_set parameter
needed to create a local message queue with the task priority
waiting queue discipline.  The attribute_set parameter to the
@code{@value{DIRPREFIX}message_queue_create} directive could be either 
@code{@value{RPREFIX}PRIORITY} or
@code{@value{RPREFIX}LOCAL @value{OR} @value{RPREFIX}PRIORITY}.  
The attribute_set parameter can be set to @code{@value{RPREFIX}PRIORITY}
because @code{@value{RPREFIX}LOCAL} is the default for all created
message queues.  If a similar message queue were to be known globally, then the
attribute_set parameter would be
@code{@value{RPREFIX}GLOBAL @value{OR} @value{RPREFIX}PRIORITY}.

@subsection Building a MESSAGE_QUEUE_RECEIVE Option Set

In general, an option is built by a bitwise OR of the
desired option components.  The set of valid options for the
@code{@value{DIRPREFIX}message_queue_receive} directive are
listed in the following table:

@itemize @bullet
@item @code{@value{RPREFIX}WAIT} - task will wait for a message (default)
@item @code{@value{RPREFIX}NO_WAIT} - task should not wait
@end itemize

An option listed as a default is not required to
appear in the option OR list, although it is a good programming
practice to specify default options.  If all defaults are
desired, the option @code{@value{RPREFIX}DEFAULT_OPTIONS} should
be specified on this call.

This example demonstrates the option parameter needed
to poll for a message to arrive.  The option parameter passed to
the @code{@value{DIRPREFIX}message_queue_receive} directive should
be @code{@value{RPREFIX}NO_WAIT}.

@section Operations

@subsection Creating a Message Queue

The @code{@value{DIRPREFIX}message_queue_create} directive creates a message
queue with the user-defined name.  The user specifies the
maximum message size and maximum number of messages which can be
placed in the message queue at one time.  The user may select
FIFO or task priority as the method for placing waiting tasks in
the task wait queue.  RTEMS allocates a Queue Control Block
(QCB) from the QCB free list to maintain the newly created queue
as well as memory for the message buffer pool associated with
this message queue.  RTEMS also generates a message queue ID
which is returned to the calling task.

For GLOBAL message queues, the maximum message size
is effectively limited to the longest message which the MPCI is
capable of transmitting.

@subsection Obtaining Message Queue IDs

When a message queue is created, RTEMS generates a
unique message queue ID.  The message queue ID may be obtained
by either of two methods.  First, as the result of an invocation
of the @code{@value{DIRPREFIX}message_queue_create} directive, the
queue ID is stored in a user provided location.  Second, the queue
ID may be obtained later using the @code{@value{DIRPREFIX}message_queue_ident}
directive.  The queue ID is used by other message manager
directives to access this message queue.

@subsection Receiving a Message

The @code{@value{DIRPREFIX}message_queue_receive} directive attempts to
retrieve a message from the specified message queue.  If at
least one message is in the queue, then the message is removed
from the queue, copied to the caller's message buffer, and
returned immediately along with the length of the message.  When
messages are unavailable, one of the following situations
applies:

@itemize @bullet
@item By default, the calling task will wait forever for the
message to arrive.

@item Specifying the @code{@value{RPREFIX}NO_WAIT} option forces an immediate return
with an error status code.

@item Specifying a timeout limits the period the task will
wait before returning with an error status.
@end itemize

If the task waits for a message, then it is placed in
the message queue's task wait queue in either FIFO or task
priority order.  All tasks waiting on a message queue are
returned an error code when the message queue is deleted.

@subsection Sending a Message

Messages can be sent to a queue with the
@code{@value{DIRPREFIX}message_queue_send} and
@code{@value{DIRPREFIX}message_queue_urgent} directives.  These
directives work identically when tasks are waiting to receive a
message.  A task is removed from the task waiting queue,
unblocked,  and the message is copied to a waiting task's
message buffer.

When no tasks are waiting at the queue,
@code{@value{DIRPREFIX}message_queue_send} places the
message at the rear of the message queue, while
@code{@value{DIRPREFIX}message_queue_urgent} places the message at the
front of the queue.  The message is copied to a message buffer
from this message queue's buffer pool and then placed in the
message queue.  Neither directive can successfully send a
message to a message queue which has a full queue of pending
messages.

@subsection Broadcasting a Message

The @code{@value{DIRPREFIX}message_queue_broadcast} directive sends the same
message to every task waiting on the specified message queue as
an atomic operation.  The message is copied to each waiting
task's message buffer and each task is unblocked.  The number of
tasks which were unblocked is returned to the caller.

@subsection Deleting a Message Queue

The @code{@value{DIRPREFIX}message_queue_delete} directive removes a message
queue from the system and frees its control block as well as the
memory associated with this message queue's message buffer pool.
A message queue can be deleted by any local task that knows the
message queue's ID.  As a result of this directive, all tasks
blocked waiting to receive a message from the message queue will
be readied and returned a status code which indicates that the
message queue was deleted.  Any subsequent references to the
message queue's name and ID are invalid.  Any messages waiting
at the message queue are also deleted and deallocated.

@section Directives

This section details the message manager's
directives.  A subsection is dedicated to each of this manager's
directives and describes the calling sequence, related
constants, usage, and status codes.

@c
@c
@c
@page
@subsection MESSAGE_QUEUE_CREATE - Create a queue

@cindex create a message queue

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_message_queue_create
@example
rtems_status_code rtems_message_queue_create(
  rtems_name        name,
  uint32_t          count,
  size_t            max_message_size,
  rtems_attribute   attribute_set,
  rtems_id         *id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Message_Queue_Create (
   Name             : in     RTEMS.Name;
   Count            : in     RTEMS.Unsigned32;
   Max_Message_Size : in     RTEMS.Unsigned32;
   Attribute_Set    : in     RTEMS.Attribute;
   ID               :    out RTEMS.ID;
   Result           :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - queue created successfully@*
@code{@value{RPREFIX}INVALID_NAME} - invalid queue name@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{id} is NULL@*
@code{@value{RPREFIX}INVALID_NUMBER} - invalid message count@*
@code{@value{RPREFIX}INVALID_SIZE} - invalid message size@*
@code{@value{RPREFIX}TOO_MANY} - too many queues created@*
@code{@value{RPREFIX}UNSATISFIED} - unable to allocate message buffers@*
@code{@value{RPREFIX}MP_NOT_CONFIGURED} - multiprocessing not configured@*
@code{@value{RPREFIX}TOO_MANY} - too many global objects

@subheading DESCRIPTION:

This directive creates a message queue which resides
on the local node with the user-defined name specified in name.
For control and maintenance of the queue, RTEMS allocates and
initializes a QCB.  Memory is allocated from the RTEMS Workspace
for the specified count of messages, each of max_message_size
bytes in length.  The RTEMS-assigned queue id, returned in id,
is used to access the message queue.

Specifying @code{@value{RPREFIX}PRIORITY} in attribute_set causes tasks
waiting for a message to be serviced according to task priority.
When @code{@value{RPREFIX}FIFO} is specified, waiting tasks are serviced
in First In-First Out order.

@subheading NOTES:

This directive will not cause the calling task to be
preempted.

The following message queue attribute constants are
defined by RTEMS:

@itemize @bullet
@item @code{@value{RPREFIX}FIFO} - tasks wait by FIFO (default)
@item @code{@value{RPREFIX}PRIORITY} - tasks wait by priority
@item @code{@value{RPREFIX}LOCAL} - local message queue (default)
@item @code{@value{RPREFIX}GLOBAL} - global message queue
@end itemize

Message queues should not be made global unless
remote tasks must interact with the created message queue.  This
is to avoid the system overhead incurred by the creation of a
global message queue.  When a global message queue is created,
the message queue's name and id must be transmitted to every
node in the system for insertion in the local copy of the global
object table.

For GLOBAL message queues, the maximum message size
is effectively limited to the longest message which the MPCI is
capable of transmitting.

The total number of global objects, including message
queues, is limited by the maximum_global_objects field in the
configuration table.

@c
@c
@c
@page
@subsection MESSAGE_QUEUE_IDENT - Get ID of a queue

@cindex get ID of a message queue

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_message_queue_ident
@example
rtems_status_code rtems_message_queue_ident(
  rtems_name  name,
  uint32_t    node,
  rtems_id   *id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Message_Queue_Ident (
   Name   : in     RTEMS.Name;
   Node   : in     RTEMS.Unsigned32;
   ID     :    out RTEMS.ID;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - queue identified successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{id} is NULL@*
@code{@value{RPREFIX}INVALID_NAME} - queue name not found@*
@code{@value{RPREFIX}INVALID_NODE} - invalid node id

@subheading DESCRIPTION:

This directive obtains the queue id associated with
the queue name specified in name.  If the queue name is not
unique, then the queue id will match one of the queues with that
name.  However, this queue id is not guaranteed to correspond to
the desired queue.  The queue id is used with other message
related directives to access the message queue.

@subheading NOTES:

This directive will not cause the running task to be
preempted.

If node is @code{@value{RPREFIX}SEARCH_ALL_NODES}, all nodes are searched
with the local node being searched first.  All other nodes are
searched with the lowest numbered node searched first.

If node is a valid node number which does not
represent the local node, then only the message queues exported
by the designated node are searched.

This directive does not generate activity on remote
nodes.  It accesses only the local copy of the global object
table.

@c
@c
@c
@page
@subsection MESSAGE_QUEUE_DELETE - Delete a queue

@cindex delete a message queue

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_message_queue_delete
@example
rtems_status_code rtems_message_queue_delete(
  rtems_id id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Message_Queue_Delete (
   ID     : in     RTEMS.ID;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - queue deleted successfully@*
@code{@value{RPREFIX}INVALID_ID} - invalid queue id@*
@code{@value{RPREFIX}ILLEGAL_ON_REMOTE_OBJECT} - cannot delete remote queue

@subheading DESCRIPTION:

This directive deletes the message queue specified by
id.  As a result of this directive, all tasks blocked waiting to
receive a message from this queue will be readied and returned a
status code which indicates that the message queue was deleted.
If no tasks are waiting, but the queue contains messages, then
RTEMS returns these message buffers back to the system message
buffer pool.  The QCB for this queue as well as the memory for
the message buffers is reclaimed by RTEMS.

@subheading NOTES:

The calling task will be preempted if its preemption
mode is enabled and one or more local tasks with a higher
priority than the calling task are waiting on the deleted queue.
The calling task will NOT be preempted if the tasks that are
waiting are remote tasks.

The calling task does not have to be the task that
created the queue, although the task and queue must reside on
the same node.

When the queue is deleted, any messages in the queue
are returned to the free message buffer pool.  Any information
stored in those messages is lost.

When a global message queue is deleted, the message
queue id must be transmitted to every node in the system for
deletion from the local copy of the global object table.

Proxies, used to represent remote tasks, are
reclaimed when the message queue is deleted.

@c
@c
@c
@page
@subsection MESSAGE_QUEUE_SEND - Put message at rear of a queue

@cindex send message to a queue

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_message_queue_send
@example
rtems_status_code rtems_message_queue_send(
  rtems_id   id,
  cons void *buffer,
  size_t     size
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Message_Queue_Send (
   ID     : in     RTEMS.ID;
   Buffer : in     RTEMS.Address;
   Size   : in     RTEMS.Unsigned32;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - message sent successfully@*
@code{@value{RPREFIX}INVALID_ID} - invalid queue id@*
@code{@value{RPREFIX}INVALID_SIZE} - invalid message size@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{buffer} is NULL@*
@code{@value{RPREFIX}UNSATISFIED} - out of message buffers@*
@code{@value{RPREFIX}TOO_MANY} - queue's limit has been reached

@subheading DESCRIPTION:

This directive sends the message buffer of size bytes
in length to the queue specified by id.  If a task is waiting at
the queue, then the message is copied to the waiting task's
buffer and the task is unblocked. If no tasks are waiting at the
queue, then the message is copied to a message buffer which is
obtained from this message queue's message buffer pool.  The
message buffer is then placed at the rear of the queue.

@subheading NOTES:

The calling task will be preempted if it has
preemption enabled and a higher priority task is unblocked as
the result of this directive.

Sending a message to a global message queue which
does not reside on the local node will generate a request to the
remote node to post the message on the specified message queue.

If the task to be unblocked resides on a different
node from the message queue, then the message is forwarded to
the appropriate node, the waiting task is unblocked, and the
proxy used to represent the task is reclaimed.

@c
@c
@c
@page
@subsection MESSAGE_QUEUE_URGENT - Put message at front of a queue

@cindex put message at front of queue

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_message_queue_urgent
@example
rtems_status_code rtems_message_queue_urgent(
  rtems_id    id,
  const void *buffer,
  size_t      size
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Message_Queue_Urgent (
   ID     : in     RTEMS.ID;
   Buffer : in     RTEMS.Address;
   Size   : in     RTEMS.Unsigned32;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - message sent successfully@*
@code{@value{RPREFIX}INVALID_ID} - invalid queue id@*
@code{@value{RPREFIX}INVALID_SIZE} - invalid message size@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{buffer} is NULL@*
@code{@value{RPREFIX}UNSATISFIED} - out of message buffers@*
@code{@value{RPREFIX}TOO_MANY} - queue's limit has been reached

@subheading DESCRIPTION:

This directive sends the message buffer of size bytes
in length to the queue specified by id.  If a task is waiting on
the queue, then the message is copied to the task's buffer and
the task is unblocked.  If no tasks are waiting on the queue,
then the message is copied to a message buffer which is obtained
from this message queue's message buffer pool.  The message
buffer is then placed at the front of the queue.

@subheading NOTES:

The calling task will be preempted if it has
preemption enabled and a higher priority task is unblocked as
the result of this directive.

Sending a message to a global message queue which
does not reside on the local node will generate a request
telling the remote node to post the message on the specified
message queue.

If the task to be unblocked resides on a different
node from the message queue, then the message is forwarded to
the appropriate node, the waiting task is unblocked, and the
proxy used to represent the task is reclaimed.

@c
@c
@c
@page
@subsection MESSAGE_QUEUE_BROADCAST - Broadcast N messages to a queue

@cindex broadcast message to a queue

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_message_queue_broadcast
@example
rtems_status_code rtems_message_queue_broadcast(
  rtems_id    id,
  const void *buffer,
  size_t      size,
  uint32_t   *count
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Message_Queue_Broadcast (
   ID     : in     RTEMS.ID;
   Buffer : in     RTEMS.Address;
   Size   : in     RTEMS.Unsigned32;
   Count  :    out RTEMS.Unsigned32;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - message broadcasted successfully@*
@code{@value{RPREFIX}INVALID_ID} - invalid queue id@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{buffer} is NULL@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{count} is NULL@*
@code{@value{RPREFIX}INVALID_SIZE} - invalid message size

@subheading DESCRIPTION:

This directive causes all tasks that are waiting at
the queue specified by id to be unblocked and sent the message
contained in buffer.  Before a task is unblocked, the message
buffer of size byes in length is copied to that task's message
buffer.  The number of tasks that were unblocked is returned in
count.

@subheading NOTES:

The calling task will be preempted if it has
preemption enabled and a higher priority task is unblocked as
the result of this directive.

The execution time of this directive is directly
related to the number of tasks waiting on the message queue,
although it is more efficient than the equivalent number of
invocations of @code{@value{DIRPREFIX}message_queue_send}.

Broadcasting a message to a global message queue
which does not reside on the local node will generate a request
telling the remote node to broadcast the message to the
specified message queue.

When a task is unblocked which resides on a different
node from the message queue, a copy of the message is forwarded
to the appropriate node,  the waiting task is unblocked, and the
proxy used to represent the task is reclaimed.

@c
@c
@c
@page
@subsection MESSAGE_QUEUE_RECEIVE - Receive message from a queue

@cindex receive message from a queue

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_message_queue_receive
@example
rtems_status_code rtems_message_queue_receive(
  rtems_id        id,
  void           *buffer,
  size_t         *size,
  rtems_option    option_set,
  rtems_interval  timeout
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Message_Queue_Receive (
   ID         : in     RTEMS.ID;
   Buffer     : in     RTEMS.Address;
   Option_Set : in     RTEMS.Option;
   Timeout    : in     RTEMS.Interval;
   Size       :    out RTEMS.Unsigned32;
   Result     :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - message received successfully@*
@code{@value{RPREFIX}INVALID_ID} - invalid queue id@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{buffer} is NULL@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{size} is NULL@*
@code{@value{RPREFIX}UNSATISFIED} - queue is empty@*
@code{@value{RPREFIX}TIMEOUT} - timed out waiting for message@*
@code{@value{RPREFIX}OBJECT_WAS_DELETED} - queue deleted while waiting

@subheading DESCRIPTION:

This directive receives a message from the message
queue specified in id.  The @code{@value{RPREFIX}WAIT} and @code{@value{RPREFIX}NO_WAIT} options of the
options parameter allow the calling task to specify whether to
wait for a message to become available or return immediately.
For either option, if there is at least one message in the
queue, then it is copied to buffer, size is set to return the
length of the message in bytes, and this directive returns
immediately with a successful return code.  The buffer has to be big enough to
receive a message of the maximum length with respect to this message queue.

If the calling task chooses to return immediately and
the queue is empty, then a status code indicating this condition
is returned.  If the calling task chooses to wait at the message
queue and the queue is empty, then the calling task is placed on
the message wait queue and blocked.  If the queue was created
with the @code{@value{RPREFIX}PRIORITY} option specified, then
the calling task is inserted into the wait queue according to
its priority.  But, if the queue was created with the
@code{@value{RPREFIX}FIFO} option specified, then the
calling task is placed at the rear of the wait queue.

A task choosing to wait at the queue can optionally
specify a timeout value in the timeout parameter.  The timeout
parameter specifies the maximum interval to wait before the
calling task desires to be unblocked.  If it is set to
@code{@value{RPREFIX}NO_TIMEOUT}, then the calling task will wait forever.

@subheading NOTES:

The following message receive option constants are
defined by RTEMS:

@itemize @bullet
@item @code{@value{RPREFIX}WAIT} - task will wait for a message (default)
@item @code{@value{RPREFIX}NO_WAIT} - task should not wait
@end itemize

Receiving a message from a global message queue which
does not reside on the local node will generate a request to the
remote node to obtain a message from the specified message
queue.  If no message is available and @code{@value{RPREFIX}WAIT} was specified, then
the task must be blocked until a message is posted.  A proxy is
allocated on the remote node to represent the task until the
message is posted.

A clock tick is required to support the timeout functionality of
this directive.

@c
@c
@c
@page
@subsection MESSAGE_QUEUE_GET_NUMBER_PENDING - Get number of messages pending on a queue

@cindex get number of pending messages

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_message_queue_get_number_pending
@example
rtems_status_code rtems_message_queue_get_number_pending(
  rtems_id  id,
  uint32_t *count
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Message_Queue_Get_Number_Pending (
   ID     : in     RTEMS.ID;
   Count  :    out RTEMS.Unsigned32;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - number of messages pending returned successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{count} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid queue id

@subheading DESCRIPTION:

This directive returns the number of messages pending on this
message queue in count.  If no messages are present
on the queue, count is set to zero.

@subheading NOTES:

Getting the number of pending messages on a global message queue which
does not reside on the local node will generate a request to the
remote node to actually obtain the pending message count for
the specified message queue.


@c
@c
@c
@page
@subsection MESSAGE_QUEUE_FLUSH - Flush all messages on a queue

@cindex flush messages on a queue

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_message_queue_flush
@example
rtems_status_code rtems_message_queue_flush(
  rtems_id  id,
  uint32_t *count
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Message_Queue_Flush (
   ID     : in     RTEMS.ID;
   Count  :    out RTEMS.Unsigned32;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - message queue flushed successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{count} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid queue id

@subheading DESCRIPTION:

This directive removes all pending messages from the
specified queue id.  The number of messages removed is returned
in count.  If no messages are present on the queue, count is set
to zero.

@subheading NOTES:

Flushing all messages on a global message queue which
does not reside on the local node will generate a request to the
remote node to actually flush the specified message queue.

