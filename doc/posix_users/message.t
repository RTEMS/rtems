@c
@c COPYRIGHT(c) 1988-2002.
@c On-Line Applications Research Corporation(OAR).
@c All rights reserved.

@chapter Message Passing Manager

@section Introduction

The message passing manager is the means to provide communication and
synchronization capabilities using POSIX message queues.

The directives provided by the message passing manager are:

@itemize @bullet
@item @code{mq_open} - Open a Message Queue
@item @code{mq_close} - Close a Message Queue
@item @code{mq_unlink} - Remove a Message Queue
@item @code{mq_send} - Send a Message to a Message Queue
@item @code{mq_receive} - Receive a Message from a Message Queue
@item @code{mq_notify} - Notify Process that a Message is Available
@item @code{mq_setattr} - Set Message Queue Attributes
@item @code{mq_getattr} - Get Message Queue Attributes
@end itemize

@section Background

@subsection Theory

Message queues are named objects that operate with readers and writers.
In addition, a message queue is a priority queue of discrete messages.
POSIX message queues offer a certain, basic amount of application access
to, and control over, the message queue geometry that can be changed.

@subsection Messages

A message is a variable length buffer where information can be stored to
support communication. The length of the message and the information
stored in that message are user-defined and can be actual data,
pointer(s), or empty. There is a maximum acceptable length for a message
that is associated with each message queue.

@subsection Message Queues

Message queues are named objects similar to the pipes of POSIX. They are
a means of communicating data between multiple processes and for passing
messages among tasks and ISRs. Message queues can contain a variable
number of messages from 0 to an upper limit that is user defined. The
maximum length of the message can be set on a per message queue basis.
Normally messages are sent and received from the message queue in FIFO
order. However, messages can also be prioritized and a priority queue
established for the passing of messages. Synchronization is needed when a
task waits for a message to arrive at a queue. Also, a task may poll a
queue for the arrival of a message.

@findex mqd_t
The message queue descriptor @code{mqd_t} represents the message queue. It is
passed as an argument to all of the message queue functions.

@subsection Building a Message Queue Attribute Set

The mq_attr structure is used to define the characteristics of the message
queue.

@findex mq_attr
@example
@group
typedef struct mq_attr@{
  long mq_flags;
  long mq_maxmsg;
  long mq_msgsize;
  long mq_curmsgs;
@};
@end group
@end example

All of these attributes are set when the message queue is created using
mq_open. The mq_flags field is not used in the creation of a message
queue, it is only used by mq_setattr and mq_getattr. The structure
mq_attr is passed as an argument to mq_setattr and mq_getattr.

The mq_flags contain information affecting the behavior of the message
queue. The O_NONBLOCK mq_flag is the only flag that is defined. In
mq_setattr, the mq_flag can be set to dynamically change the blocking and
non-blocking behavior of the message queue. If the non-block flag is set
then the message queue is non-blocking, and requests to send and receive
messages do not block waiting for resources. For a blocking message
queue, a request to send might have to wait for an empty message queue,
and a request to receive might have to wait for a message to arrive on the
queue. Both mq_maxmsg and mq_msgsize affect the sizing of the message
queue. mq_maxmsg specifies how many messages the queue can hold at any
one time. mq_msgsize specifies the size of any one message on the queue.
If either of these limits is exceeded, an error message results.

Upon return from mq_getattr, the mq_curmsgs is set according to the
current state of the message queue. This specifies the number of messages
currently on the queue.

@subsection Notification of a Message on the Queue

Every message queue has the ability to notify one (and only one) process
whenever the queue's state changes from empty (0 messages) to nonempty.
This means that the process does not have to block or constantly poll
while it waits for a message. By calling mq_notify, you can attach a
notification request to a message queue. When a message is received by an
empty queue, if there are no processes blocked and waiting for the
message, then the queue notifies the requesting process of a message
arrival. There is only one signal sent by the message queue, after that
the notification request is de-registered and another process can attach
its notification request. After receipt of a notification, a process must
re-register if it wishes to be notified again.

If there is a process blocked and waiting for the message, that process
gets the message, and notification is not sent. It is also possible for
another process to receive the message after the notification is sent but
before the notified process has sent its receive request.

Only one process can have a notification request attached to a message
queue at any one time. If another process attempts to register a
notification request, it fails. You can de-register for a message queue
by passing a NULL to mq_notify, this removes any notification request
attached to the queue. Whenever the message queue is closed, all
notification attachments are removed.

@subsection POSIX Interpretation Issues

There is one significant point of interpretation related to 
the RTEMS implementation of POSIX message queues:

@cite{What happens to threads already blocked on a message queue when the
mode of that same message queue is changed from blocking to non-blocking?}


The RTEMS POSIX implementation decided to unblock all waiting tasks
with an @code{EAGAIN} status just as if a non-blocking version of
the same operation had returned unsatisfied.  This case is not
discussed in the POSIX standard and other implementations may have
chosen alternative behaviors.

@section Operations

@subsection Opening or Creating a Message Queue

If the message queue already exists, mq_open() opens it, if the message
queue does not exist, mq_open() creates it. When a message queue is
created, the geometry of the message queue is contained in the attribute
structure that is passed in as an argument. This includes mq_msgsize that
dictates the maximum size of a single message, and the mq_maxmsg that
dictates the maximum number of messages the queue can hold at one time.
The blocking or non-blocking behavior of the queue can also specified.

@subsection Closing a Message Queue

The mq_close() function is used to close the connection made to a message
queue that was made during mq_open. The message queue itself and the
messages on the queue are persistent and remain after the queue is closed.

@subsection Removing a Message Queue

The mq_unlink() function removes the named message queue. If the message
queue is not open when mq_unlink is called, then the queue is immediately
eliminated. Any messages that were on the queue are lost, and the queue
can not be opened again. If processes have the queue open when mq_unlink
is called, the removal of the queue is delayed until the last process
using the queue has finished. However, the name of the message queue is
removed so that no other process can open it.

@subsection Sending a Message to a Message Queue

The mq_send() function adds the message in priority order to the message
queue. Each message has an assigned a priority. The highest priority
message is be at the front of the queue.

The maximum number of messages that a message queue may accept is
specified at creation by the mq_maxmsg field of the attribute structure.
If this amount is exceeded, the behavior of the process is determined
according to what oflag was used when the message queue was opened. If
the queue was opened with O_NONBLOCK flag set, the process does not block,
and an error is returned. If the O_NONBLOCK flag was not set, the process
does block and wait for space on the queue.

@subsection Receiving a Message from a Message Queue

The mq_receive() function is used to receive the oldest of the highest
priority message(s) from the message queue specified by mqdes. The
messages are received in FIFO order within the priorities. The received
message's priority is stored in the location referenced by the msg_prio.
If the msg_prio is a NULL, the priority is discarded. The message is
removed and stored in an area pointed to by msg_ptr whose length is of
msg_len. The msg_len must be at least equal to the mq_msgsize attribute
of the message queue.

The blocking behavior of the message queue is set by O_NONBLOCK at mq_open
or by setting O_NONBLOCK in mq_flags in a call to mq_setattr. If this is
a blocking queue, the process does block and wait on an empty queue. If
this a non-blocking queue, the process does not block. Upon successful
completion, mq_receive returns the length of the selected message in bytes
and the message is removed from the queue.

@subsection Notification of Receipt of a Message on an Empty Queue

The mq_notify() function registers the calling process to be notified of
message arrival at an empty message queue. Every message queue has the
ability to notify one (and only one) process whenever the queue's state
changes from empty (0 messages) to nonempty. This means that the process
does not have to block or constantly poll while it waits for a message.
By calling mq_notify, a notification request is attached to a message
queue. When a message is received by an empty queue, if there are no
processes blocked and waiting for the message, then the queue notifies the
requesting process of a message arrival. There is only one signal sent by
the message queue, after that the notification request is de-registered
and another process can attach its notification request. After receipt of
a notification, a process must re-register if it wishes to be notified
again.

If there is a process blocked and waiting for the message, that process
gets the message, and notification is not sent. Only one process can have
a notification request attached to a message queue at any one time. If
another process attempts to register a notification request, it fails.
You can de-register for a message queue by passing a NULL to mq_notify,
this removes any notification request attached to the queue. Whenever the
message queue is closed, all notification attachments are removed.

@subsection Setting the Attributes of a Message Queue

The mq_setattr() function is used to set attributes associated with the
open message queue description referenced by the message queue descriptor
specified by mqdes. The *omqstat represents the old or previous
attributes. If omqstat is non-NULL, the function mq_setattr() stores, in
the location referenced by omqstat, the previous message queue attributes
and the current queue status. These values are the same as would be
returned by a call to mq_getattr() at that point.

There is only one mq_attr.mq_flag that can be altered by this call. This
is the flag that deals with the blocking and non-blocking behavior of the
message queue. If the flag is set then the message queue is non-blocking,
and requests to send or receive do not block while waiting for resources.
If the flag is not set, then message send and receive may involve waiting
for an empty queue or waiting for a message to arrive.

@subsection Getting the Attributes of a Message Queue

The mq_getattr() function is used to get status information and attributes
of the message queue associated with the message queue descriptor. The
results are returned in the mq_attr structure referenced by the mqstat
argument. All of these attributes are set at create time, except the
blocking/non-blocking behavior of the message queue which can be
dynamically set by using mq_setattr. The attribute mq_curmsg is set to
reflect the number of messages on the queue at the time that mq_getattr
was called.

@section Directives

This section details the message passing manager's directives. A
subsection is dedicated to each of this manager's directives and describes
the calling sequence, related constants, usage, and status codes.

@c
@c
@c
@page
@subsection mq_open - Open a Message Queue

@findex mq_open
@cindex  open a message queue

@subheading CALLING SEQUENCE:

@example
#include <mqueue.h>

mqd_t mq_open(
  const char     *name,
  int             oflag,
  mode_t          mode,
  struct mq_attr *attr
);
@end example

@subheading STATUS CODES:

@code{EACCES} - Either the message queue exists and the permissions
requested in oflags were denied, or the message does not exist and
permission to create one is denied.

@code{EEXIST} - You tried to create a message queue that already exists.

@code{EINVAL} - An inappropriate name was given for the message queue, or
the values of mq-maxmsg or mq_msgsize were less than 0.

@code{ENOENT} - The message queue does not exist, and you did not specify
to create it.

@code{EINTR} - The call to mq_open was interrupted by a signal.

@code{EMFILE} - The process has too many files or message queues open.
This is a process limit error.

@code{ENFILE} - The system has run out of resources to support more open
message queues. This is a system error.

@code{ENAMETOOLONG} - mq_name is too long.

@subheading DESCRIPTION:

The mq_open () function establishes the connection between a process and a
message queue with a message queue descriptor. If the message queue
already exists, mq_open opens it, if the message queue does not exist,
mq_open creates it. Message queues can have multiple senders and
receivers. If mq_open is successful, the function returns a message queue
descriptor. Otherwise, the function returns a -1 and sets 'errno' to
indicate the error.

The name of the message queue is used as an argument. For the best of
portability, the name of the message queue should begin with a "/" and no
other "/" should be in the name. Different systems interpret the name in
different ways.

The oflags contain information on how the message is opened if the queue
already exists. This may be O_RDONLY for read only, O_WRONLY for write
only, of O_RDWR, for read and write.

In addition, the oflags contain information needed in the creation of a
message queue. @code{O_NONBLOCK} - If the non-block flag is set then the
message queue is non-blocking, and requests to send and receive messages
do not block waiting for resources. If the flag is not set then the
message queue is blocking, and a request to send might have to wait for an
empty message queue. Similarly, a request to receive might have to wait
for a message to arrive on the queue. @code{O_CREAT} - This call specifies
that the call the mq_open is to create a new message queue. In this case
the mode and attribute arguments of the function call are utilized. The
message queue is created with a mode similar to the creation of a file,
read and write permission creator, group, and others.

The geometry of the message queue is contained in the attribute structure.
This includes mq_msgsize that dictates the maximum size of a single
message, and the mq_maxmsg that dictates the maximum number of messages
the queue can hold at one time. If a NULL is used in the mq_attr
argument, then the message queue is created with implementation defined
defaults. @code{O_EXCL} - is always set if O_CREAT flag is set. If the
message queue already exists, O_EXCL causes an error message to be
returned, otherwise, the new message queue fails and appends to the
existing one.

@subheading NOTES:

The mq_open () function does not add or remove messages from the queue.
When a new message queue is being created, the mq_flag field of the
attribute structure is not used.

@c
@c
@c
@page
@subsection mq_close - Close a Message Queue

@findex mq_close
@cindex  close a message queue

@subheading CALLING SEQUENCE:

@example
#include <mqueue.h>

int mq_close(
  mqd_t mqdes
);
@end example

@subheading STATUS CODES:

@code{EINVAL} - The descriptor does not represent a valid open message
queue

@subheading DESCRIPTION:

The mq_close function removes the association between the message queue
descriptor, mqdes, and its message queue. If mq_close() is successfully
completed, the function returns a value of zero; otherwise, the function
returns a value of -1 and sets errno to indicate the error.

@subheading NOTES:

If the process had successfully attached a notification request to the
message queue via mq_notify, this attachment is removed, and the message
queue is available for another process to attach for notification.
mq_close has no effect on the contents of the message queue, all the
messages that were in the queue remain in the queue.

@c
@c
@c
@page
@subsection mq_unlink - Remove a Message Queue

@findex mq_unlink
@cindex  remove a message queue

@subheading CALLING SEQUENCE:

@example
#include <mqueue.h>

int mq_unlink(
  const char *name
);
@end example

@subheading STATUS CODES:

@code{EINVAL} - The descriptor does not represent a valid message queue

@subheading DESCRIPTION:

The mq_unlink() function removes the named message queue. If the message
queue is not open when mq_unlink is called, then the queue is immediately
eliminated. Any messages that were on the queue are lost, and the queue
can not be opened again. If processes have the queue open when mq_unlink
is called, the removal of the queue is delayed until the last process
using the queue has finished. However, the name of the message queue is
removed so that no other process can open it. Upon successful completion,
the function returns a value of zero. Otherwise, the named message queue
is not changed by this function call, and the function returns a value of
-1 and sets errno to indicate the error.

@subheading NOTES:

Calls to mq_open() to re-create the message queue may fail until the
message queue is actually removed. However, the mq_unlink() call need not
block until all references have been closed; it may return immediately.

@c
@c
@c
@page
@subsection mq_send - Send a Message to a Message Queue

@findex mq_send
@cindex  send a message to a message queue

@subheading CALLING SEQUENCE:

@example
#include<mqueue.h>
int mq_send(
  mqd_t        mqdes,
  const char  *msg_ptr,
  size_t       msg_len,
  unsigned int msg_prio
);
@end example

@subheading STATUS CODES:

@code{EBADF} - The descriptor does not represent a valid message queue, or the queue was opened for read only O_RDONLY
@code{EINVAL} - The value of msg_prio was greater than the MQ_PRIO_MAX.
@code{EMSGSIZE} - The msg_len is greater than the mq_msgsize attribute of the message queue
@code{EAGAIN} - The message queue is non-blocking, and there is no room on the queue for another message as specified by the mq_maxmsg.
@code{EINTR} - The message queue is blocking. While the process was waiting for free space on the queue, a signal arrived that interrupted the wait.

@subheading DESCRIPTION:

The mq_send() function adds the message pointed to by the argument msg_ptr
to the message queue specified by mqdes. Each message is assigned a
priority , from 0 to MQ_PRIO_MAX. MQ_PRIO_MAX is defined in <limits.h> and
must be at least 32. Messages are added to the queue in order of their
priority. The highest priority message is at the front of the queue.

The maximum number of messages that a message queue may accept is
specified at creation by the mq_maxmsg field of the attribute structure.
If this amount is exceeded, the behavior of the process is determined
according to what oflag was used when the message queue was opened. If
the queue was opened with O_NONBLOCK flag set, then the EAGAIN error is
returned. If the O_NONBLOCK flag was not set, the process blocks and
waits for space on the queue, unless it is interrupted by a signal.

Upon successful completion, the mq_send () function returns a value of
zero. Otherwise, no message is enqueued, the function returns -1, and
errno is set to indicate the error.

@subheading NOTES:

If the specified message queue is not full, mq_send inserts the message at
the position indicated by the msg_prio argument.

@c
@c
@c
@page
@subsection mq_receive - Receive a Message from a Message Queue

@findex mq_receive
@cindex  receive a message from a message queue

@subheading CALLING SEQUENCE:

@example
#include <mqueue.h>

size_t mq_receive(
  mqd_t         mqdes,
  char         *msg_ptr,
  size_t        msg_len,
  unsigned int *msg_prio
);
@end example

@subheading STATUS CODES:

@code{EBADF} - The descriptor does not represent a valid message queue, or the queue was opened for write only O_WRONLY
@code{EMSGSIZE} - The msg_len is less than the mq_msgsize attribute of the message queue
@code{EAGAIN} - The message queue is non-blocking, and the queue is empty
@code{EINTR} - The message queue is blocking. While the process was waiting for a message to arrive on the queue, a signal arrived that interrupted the wait.

@subheading DESCRIPTION:

The mq_receive function is used to receive the oldest of the highest
priority message(s) from the message queue specified by mqdes. The
messages are received in FIFO order within the priorities. The received
message's priority is stored in the location referenced by the msg_prio.
If the msg_prio is a NULL, the priority is discarded. The message is
removed and stored in an area pointed to by msg_ptr whose length is of
msg_len. The msg_len must be at least equal to the mq_msgsize attribute
of the message queue.

The blocking behavior of the message queue is set by O_NONBLOCK at mq_open
or by setting O_NONBLOCK in mq_flags in a call to mq_setattr. If this is
a blocking queue, the process blocks and waits on an empty queue. If this
a non-blocking queue, the process does not block.

Upon successful completion, mq_receive returns the length of the selected
message in bytes and the message is removed from the queue. Otherwise, no
message is removed from the queue, the function returns a value of -1, and
sets errno to indicate the error.

@subheading NOTES:

If the size of the buffer in bytes, specified by the msg_len argument, is
less than the mq_msgsize attribute of the message queue, the function
fails and returns an error

@c
@c
@c
@page
@subsection mq_notify - Notify Process that a Message is Available

@findex mq_notify
@cindex  notify process that a message is available

@subheading CALLING SEQUENCE:

@example
#include <mqueue.h>

int mq_notify(
  mqd_t                  mqdes,
  const struct sigevent *notification
);
@end example

@subheading STATUS CODES:

@code{EBADF} - The descriptor does not refer to a valid message queue
@code{EBUSY} - A notification request is already attached to the queue

@subheading DESCRIPTION:

If the argument notification is not NULL, this function registers the
calling process to be notified of message arrival at an empty message
queue associated with the specified message queue descriptor, mqdes.

Every message queue has the ability to notify one (and only one) process
whenever the queue's state changes from empty (0 messages) to nonempty.
This means that the process does not have to block or constantly poll
while it waits for a message. By calling mq_notify, a notification
request is attached to a message queue. When a message is received by an
empty queue, if there are no processes blocked and waiting for the
message, then the queue notifies the requesting process of a message
arrival. There is only one signal sent by the message queue, after that
the notification request is de-registered and another process can attach
its notification request. After receipt of a notification, a process must
re-register if it wishes to be notified again.

If there is a process blocked and waiting for the message, that process
gets the message, and notification is not be sent. Only one process can
have a notification request attached to a message queue at any one time.
If another process attempts to register a notification request, it fails.
You can de-register for a message queue by passing a NULL to mq_notify;
this removes any notification request attached to the queue. Whenever the
message queue is closed, all notification attachments are removed.

Upon successful completion, mq_notify returns a value of zero; otherwise,
the function returns a value of -1 and sets errno to indicate the error.

@subheading NOTES:

It is possible for another process to receive the message after the notification is sent but before the notified process has sent its receive request.

@c
@c
@c
@page
@subsection mq_setattr - Set Message Queue Attributes

@findex mq_setattr
@cindex  set message queue attributes

@subheading CALLING SEQUENCE:

@example
#include <mqueue.h>

int mq_setattr(
  mqd_t                 mqdes,
  const struct mq_attr *mqstat,
  struct mq_attr       *omqstat
);
@end example

@subheading STATUS CODES:

@code{EBADF} - The message queue descriptor does not refer to a valid, open queue.
@code{EINVAL} - The mq_flag value is invalid.

@subheading DESCRIPTION:

The mq_setattr function is used to set attributes associated with the open
message queue description referenced by the message queue descriptor
specified by mqdes. The *omqstat represents the old or previous
attributes. If omqstat is non-NULL, the function mq_setattr() stores, in
the location referenced by omqstat, the previous message queue attributes
and the current queue status. These values are the same as would be
returned by a call to mq_getattr() at that point.

There is only one mq_attr.mq_flag which can be altered by this call.
This is the flag that deals with the blocking and non-blocking behavior of
the message queue. If the flag is set then the message queue is
non-blocking, and requests to send or receive do not block while waiting
for resources. If the flag is not set, then message send and receive may
involve waiting for an empty queue or waiting for a message to arrive.

Upon successful completion, the function returns a value of zero and the
attributes of the message queue have been changed as specified.
Otherwise, the message queue attributes is unchanged, and the function
returns a value of -1 and sets errno to indicate the error.

@subheading NOTES:

All other fields in the mq_attr are ignored by this call.

@c
@c
@c
@page
@subsection mq_getattr - Get Message Queue Attributes

@findex mq_getattr
@cindex  get message queue attributes

@subheading CALLING SEQUENCE:

@example
#include <mqueue.h>
int mq_getattr(
  mqd_t mqdes,
  struct mq_attr *mqstat
);
@end example

@subheading STATUS CODES:

@code{EBADF} - The message queue descriptor does not refer to a valid,
open message queue.

@subheading DESCRIPTION:

The mqdes argument specifies a message queue descriptor. The mq_getattr
function is used to get status information and attributes of the message
queue associated with the message queue descriptor. The results are
returned in the mq_attr structure referenced by the mqstat argument. All
of these attributes are set at create time, except the
blocking/non-blocking behavior of the message queue which can be
dynamically set by using mq_setattr. The attribute mq_curmsg is set to
reflect the number of messages on the queue at the time that mq_getattr
was called.

Upon successful completion, the mq_getattr function returns zero.
Otherwise, the function returns -1 and sets errno to indicate the error.

@subheading NOTES:

