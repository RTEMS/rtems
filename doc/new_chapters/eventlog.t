@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Event Logging Manager

@section Introduction

The event logging manager provides a portable method for logging
system and application events and subsequent processing of those
events.  The capabilities in this manager were defined in the POSIX
1003.1h/D3 proposed standard titled @b{Services for Reliable,
Available, and Serviceable Systems}.

The directives provided by the event logging manager are:

@itemize @bullet
@item @code{log_write} - Write to the system Log
@item @code{log_write_any} - Write to any log file
@item @code{log_write_entry} - Write entry to any log file
@item @code{log_open} - Open a log file
@item @code{log_read} - Read from a log file
@item @code{log_notify} - Notify Process of writes to the system log
@item @code{log_close} - Close log descriptor
@item @code{log_seek} - Reposition log file offset
@item @code{log_severity_before} - Compare event record severities
@item @code{log_facilityemptyset} - Manipulate log facility sets
@item @code{log_facilityfillset} - Manipulate log facility sets
@item @code{log_facilityaddset} - Manipulate log facility sets
@item @code{log_facilitydelset} - Manipulate log facility sets
@item @code{log_facilityismember} - Manipulate log facility sets
@end itemize

@section Background

@subsection Log Files and Events

System log

Non-system logs

Events, facility, severity

@subsection Queries

@section Operations

@subsection Creating and Writing a non-System Log

Discuss creating and writing to a non-system log.

@example
  log_create
  log_write loop
@end example

@subsection Reading a Log

Discuss opening and reading from a log.

@example
  build a query
  log_open
  log_read loop
@end example

@section Directives

This section details the event logging manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection log_write - Write to the system Log

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_write(
  const log_facility_t  facility,
  const int             event_id,
  const log_severity_t  severity,
  const void           *buf,
  const size_t          len
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E2BIG
This error indicates an inconsistency in the implementation.
Report this as a bug.

@item EINVAL
The @code{facility} argument is not a valid log facility.

@item EINVAL
The @code{severity} argument exceeds @code{LOG_SEVERITY_MAX}.

@item EINVAL
The @code{len} argument exceeds @code{LOG_MAXIUM_BUFFER_SIZE}.

@item EINVAL
The @code{len} argument was non-zero and @code{buf} is NULL.

@item ENOSPC
The device which contains the log file has run out of space.

@item EIO
An I/O error occurred in writing to the log file.

@end table

@subheading DESCRIPTION:

The @code{log_write_any} function writes an event record to the 
system log file.  The event record written consists of the
event attributes specified by the @code{facility}, @code{event_id},
and @code{severity} arguments as well as the data identified by the
@code{buf} and @code{len} arguments.  The fields of the event record
structure to be written are filled in as follows:

@table @b
@item log_recid
This is set to a monotonically increasing log record id
maintained by the system for this individual log file.

@item log_size
This is set to the value of the @code{len} argument.

@item log_event_id
This is set to the value of the @code{event_id} argument.

@item log_facility
This is set to the value of the @code{facility} argument.

@item log_severity
This is set to the value of the @code{severity} argument.

@item log_uid
This is set to the value returned by @code{geteuid}.

@item log_gid
This is set to the value returned by @code{getegid}.

@item log_pid
This is set to the value returned by @code{getpid}.

@item log_pgrp
This is set to the value returned by @code{getpgrp}.

@item log_time
This is set to the value returned by @code{clock_gettime} for the 
CLOCK_REALTIME clock source.

@end table

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

This implementation can not return the @code{EPERM} error.

@page
@subsection log_write_any - Write to the any log file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_write_any(
  const logd_t          logdes,
  const log_facility_t  facility,
  const int             event_id,
  const log_severity_t  severity,
  const void           *buf,
  const size_t          len
);
@end example
@end ifset
 
@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E2BIG
This error indicates an inconsistency in the implementation.
Report this as a bug.

@item EBADF
The @code{logdes} argument is not a valid log descriptor.

@item EINVAL
The @code{facility} argument is not a valid log facility.

@item EINVAL
The @code{severity} argument exceeds @code{LOG_SEVERITY_MAX}.

@item EINVAL
The @code{len} argument exceeds @code{LOG_MAXIMUM_BUFFER_SIZE}.

@item EINVAL
The @code{len} argument was non-zero and @code{buf} is NULL.

@item ENOSPC
The device which contains the log file has run out of space.

@item EIO
An I/O error occurred in writing to the log file.

@end table

@subheading DESCRIPTION:

The @code{log_write_any} function writes an event record to the log file
specified by @code{logdes}.  The event record written consists of the
event attributes specified by the @code{facility}, @code{event_id},
and @code{severity} arguments as well as the data identified by the
@code{buf} and @code{len} arguments.  The fields of the event record
structure to be written are filled in as follows:

@table @b
@item log_recid
This is set to a monotonically increasing log record id
maintained by the system for this individual log file.

@item log_size
This is set to the value of the @code{len} argument.

@item log_event_id
This is set to the value of the @code{event_id} argument.

@item log_facility
This is set to the value of the @code{facility} argument.

@item log_severity
This is set to the value of the @code{severity} argument.

@item log_uid
This is set to the value returned by @code{geteuid}.

@item log_gid
This is set to the value returned by @code{getegid}.

@item log_pid
This is set to the value returned by @code{getpid}.

@item log_pgrp
This is set to the value returned by @code{getpgrp}.

@item log_time
This is set to the value returned by @code{clock_gettime} for the 
CLOCK_REALTIME clock source.

@end table

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

This implementation can not return the @code{EPERM} error.

This function is not defined in the POSIX specification.  It is 
an extension provided by this implementation.

@page
@subsection log_write_entry - Write entry to any log file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_write_entry(
  const logd_t      logdes,
  struct log_entry *entry,
  const void       *buf,
  const size_t      len
);
@end example
@end ifset
 
@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E2BIG
This error indicates an inconsistency in the implementation.
Report this as a bug.

@item EBADF
The @code{logdes} argument is not a valid log descriptor.

@item EFAULT
The @code{entry} argument is not a valid pointer to a log entry.

@item EINVAL
The @code{facility} field in @code{entry} is not a valid log facility.

@item EINVAL
The @code{severity} field in @code{entry} exceeds @code{LOG_SEVERITY_MAX}.

@item EINVAL
The @code{len} argument exceeds @code{LOG_MAXIMUM_BUFFER_SIZE}.

@item EINVAL
The @code{len} argument was non-zero and @code{buf} is NULL.

@item ENOSPC
The device which contains the log file has run out of space.

@item EIO
An I/O error occurred in writing to the log file.

@end table

@subheading DESCRIPTION:

The @code{log_write_entry} function writes an event record 
specified by the @code{entry}, @code{buf}, and @code{len} arguments.
Most of the fields of the event record pointed to by @code{entry}
are left intact.  The following fields are filled in as follows:

@table @b
@item log_recid
This is set to a monotonically increasing log record id
maintained by the system for this individual log file.

@item log_size
This is set to the value of the @code{len} argument.

@end table

This allows existing log entries from one log file to be written to 
another log file without destroying the logged information.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

This implementation can not return the @code{EPERM} error.

This function is not defined in the POSIX specification.  It is 
an extension provided by this implementation.

@page
@subsection log_open - Open a log file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_open(
  logd_t               *logdes,
  const char           *path,
  const log_query_t    *query
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission is denied on a component of the path prefix,
or the log file exists and read permission is denied.

@item  EINTR
A signal interrupted the call to log_open().

@item EINVAL
The log_severity field of the query argument exceeds 
@code{LOG_SEVERITY_MAX}.

@item EINVAL
The path argument referred to a file that was not a log file.

@item EMFILE
Too many log file descriptors are currently in use by this
process.

@item ENAMETOOLONG
The length of the path string exceeds @code{PATH_MAX}, or a pathname
component is longer than @code{NAME_MAX} while @code{_POSIX_NO_TRUNC} is
in effect.

@item ENFILE
Too many files are currently open in the system.

@item ENOENT
The file specified by the path argument does not exist.

@item ENOTDIR
A component of the path prefix is not a directory.

@end table

@subheading DESCRIPTION:

The @code{log_open} function establishes the connection between a 
log file and a log file descriptor.  It creates an open log file 
descriptor that refers to this query stream on the specified log file
The log file descriptor is used by the other log functions to refer
to that log query stream.  The @code{path} argument points to a
pathname for a log file.  A @code{path} argument of NULL specifies
the current system log file.  

The @code{query} argument is not NULL, then it points to a log query
specification that is used to filter the records in the log file on
subsequent @code{log_read} operations.  This restricts the set of
event records read using the returned log file descriptor to those
which match the query.  A query match occurs for a given record when
that record's facility is a member of the query's facility set and
the record's severity is greater than or equal to the severity specified
in the query.

If the value of the @code{query} argument is NULL, no query filter
shall be applied.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

POSIX specifies that @code{EINVAL} will be returned if the 
@code{log_facilities} field of the @code{query} argument is not
a valid facility set.  In this implementation, this condition
can never occur.

Many error codes that POSIX specifies to be returned by @code{log_open}
should actually be detected by @code{open} and passed back by the
@code{log_open} implementation.  In this implementation, @code{EACCESS},
@code{EMFILE}, @code{ENAMETOOLONG}, @code{ENFILE}, @code{ENOENT},
and @code{ENOTDIR} are detected in this manner.

@page
@subsection log_read - Read from a log file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_read(
  const logd_t      logdes,
  struct log_entry *entry,
  void             *log_buf,
  const size_t      log_len,
  const size_t     *log_sizeread
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E2BIG
This error indicates an inconsistency in the implementation.
Report this as a bug.

@item EBADF
The @code{logdes} argument is not a valid log file descriptor.

@item EFAULT
The @code{entry} argument is not a valid pointer to a log entry structure.

@item EFAULT
The @code{log_sizeread} argument is not a valid pointer to a size_t.

@item EBUSY
No data available.  There are no unread event records remaining
in this log file.

@item EINTR
A signal interrupted the call to log_read().

@item EIO
An I/O error occurred in reading from the event log.

@item EINVAL
The matching event record has data associated with it and
@code{log_buf} was not a valid pointer.

@item EINVAL
The matching event record has data associated with it which is
longer than @code{log_len}.

@end table

@subheading DESCRIPTION:

The @code{log_read} function reads the @code{log_entry}
structure and up to @code{log_len} bytes of data from the next
event record of the log file associated with the open log file
descriptor @code{logdes}.  The event record read is placed
into the @code{log_entry} structure pointed to by
@code{entry} and any data into the buffer pointed to by @code{log_buf}.
The log record ID of the returned event record is be stored in the 
@code{log_recid} member of the @code{log_entry} structure for the event
record.

If the query attribute of the open log file description associated with
the @code{logdes} is set, the event record read will match that query.

If the @code{log_read} is successful the call stores the actual length
of the data associated with the event record into the location specified by
@code{log_sizeread}.  This number will be less than or equal to
@code{log_len}.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

When @code{EINVAL} is returned, then no data is returned although the
event record is returned.  This is an extension to the POSIX specification.

The POSIX specification specifically allows @code{log_read} to write
greater than @code{log_len} bytes into @code{log_buf}.  This is highly
undesirable and this implementation will NOT do this.

@page
@subsection log_notify - Notify Process of writes to the system log.

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_notify(
  const logd_t           logdes,
  const struct sigevent *notification
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
The logdes argument is not a valid log file descriptor.

@item EINVAL
The notification argument specifies an invalid signal.

@item EINVAL
The process has requested a notify on a log that will not be
written to.

@item ENOSYS
The function log_notify() is not supported by this implementation.

@end table

@subheading DESCRIPTION:

If the argument @code{notification} is not NULL this function registers 
the calling process to be notified of event records received by the system 
log, which match the query parameters associated with the open log descriptor
specified by @code{logdes}.  The notification specified by the 
@code{notification} argument shall be sent to the process when an event 
record received by the system log is matched by the query attribute of the 
open log file description associated with the @code{logdes} log file 
descriptor.  If the calling process has already registered a notification
for the @code{logdes} log file descriptor, the new notification shall 
replace the existing notification registration.

If the @code{notification} argument is NULL and the calling process is 
currently registered to be notified for the @code{logdes} log file
descriptor, the existing registration shall be removed.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

@page
@subsection log_close - Close log descriptor

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_close(
  const logd_t   logdes
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
The logdes argument is not a valid log file descriptor.

@end table

@subheading DESCRIPTION:

The @code{log_close} function deallocates the open log file descriptor 
indicated by @code{log_des}.

When all log file descriptors associated with an open log file description
have been closed, the open log file description is freed.

If the link count of the log file is zero, when all log file descriptors
have been closed, the space occupied by the log file is freed and the
log file shall no longer be accessible.

If the process has successfully registered a notification request for the 
log file descriptor, the registration is removed. 

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

@page
@subsection log_seek - Reposition log file offset

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_seek(
  const logd_t    logdes,
  log_recid_t     log_recid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
The @code{logdes} argument is not a valid log file descriptor.

@end table

@subheading DESCRIPTION:

The @code{log_seek} function sets the log file offset of the open 
log description associated with the @code{logdes} log file descriptor 
to the event record in the log file identified by @code{log_recid}.  
The @code{log_recid} argument is either the record id of a valid event
record or one of the following values, as defined in the header <evlog.h>:

@table @b
@item LOG_SEEK_START
Set log file position to point at the first event
record in the log file.

@item LOG_SEEK_END
Set log file position to point after the last event 
record in the log file.

@end table

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

This implementation can not return @code{EINTR}.

This implementation can not return @code{EINVAL} to indicate that
the @code{log_recid} argument is not a valid record id.

@page
@subsection log_severity_before - Compare event record severities

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_severity_before(
  log_severity_t  s1,
  log_severity_t  s2
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item 0
The severity of @code{s1} is less than that of @code{s2}.

@item 1
The severity of @code{s1} is greater than or equal that of @code{s2}.

@item EINVAL 
The value of either s1 or s2 exceeds @code{LOG_SEVERITY_MAX}.

@end table

@subheading DESCRIPTION:

The @code{log_severity_before} function compares the severity order
of the @code{s1} and @code{s2} arguments.  If @code{s1} is of 
severity greater than or equal to that of @code{s2}, then this
function returns 1.  Otherwise, it returns 0.

If either @code{s1} or @code{s2} specify invalid severity values, the
return value of @code{log_severity_before} is unspecified.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

The POSIX specification of the return value for this function is ambiguous.
If @code{EINVAL} is equal to 1 in an implementation, then the application
can not distinguish between greater than and an error condition.

@page
@subsection log_facilityemptyset - Manipulate log facility sets

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_facilityemptyset(
  log_facility_set_t  *set
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EFAULT
The @code{set} argument is an invalid pointer.

@end table

@subheading DESCRIPTION:

The @code{log_facilityemptyset} function initializes the facility
set pointed to by the argument @code{set}, such that all facilities
are excluded.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

Applications shall call either @code{log_facilityemptyset} or 
@code{log_facilityfillset} at least once for each object of type
@code{log_facilityset_t} prior to any other use of that object.  If
such an object is not initialized in this way, but is nonetheless 
supplied as an argument to any of the @code{log_facilityaddset}, 
@code{logfacilitydelset}, @code{log_facilityismember} or 
@code{log_open} functions, the results are undefined.

@page
@subsection log_facilityfillset - Manipulate log facility sets

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_facilityfillset(
  log_facility_set_t  *set
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EFAULT
The @code{set} argument is an invalid pointer.

@end table

@subheading DESCRIPTION:

The @code{log_facilityfillset} function initializes the facility
set pointed to by the argument @code{set}, such that all facilities
are included.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

Applications shall call either @code{log_facilityemptyset} or 
@code{log_facilityfillset} at least once for each object of type
@code{log_facilityset_t} prior to any other use of that object.  If
such an object is not initialized in this way, but is nonetheless 
supplied as an argument to any of the @code{log_facilityaddset}, 
@code{logfacilitydelset}, @code{log_facilityismember} or 
@code{log_open} functions, the results are undefined.

@page
@subsection log_facilityaddset - Manipulate log facility sets

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_facilityaddset(
  log_facility_set_t  *set,
  log_facility_t       facilityno
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EFAULT
The @code{set} argument is an invalid pointer.

@item EINVAL
The @code{facilityno} argument is not a valid facility.

@end table

@subheading DESCRIPTION:

The @code{log_facilityaddset} function adds the individual
facility specified by the value of the argument @code{facilityno}
to the facility set pointed to by the argument @code{set}.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

Applications shall call either @code{log_facilityemptyset} or 
@code{log_facilityfillset} at least once for each object of type
@code{log_facilityset_t} prior to any other use of that object.  If
such an object is not initialized in this way, but is nonetheless 
supplied as an argument to any of the @code{log_facilityaddset}, 
@code{logfacilitydelset}, @code{log_facilityismember} or 
@code{log_open} functions, the results are undefined.

@page
@subsection log_facilitydelset - Manipulate log facility sets

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_facilitydelset(
  log_facility_set_t  *set,
  log_facility_t       facilityno
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EFAULT
The @code{set} argument is an invalid pointer.

@item EINVAL
The @code{facilityno} argument is not a valid facility.

@end table

@subheading DESCRIPTION:

The @code{log_facilitydelset} function deletes the individual
facility specified by the value of the argument @code{facilityno}
from the facility set pointed to by the argument @code{set}.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

Applications shall call either @code{log_facilityemptyset} or 
@code{log_facilityfillset} at least once for each object of type
@code{log_facilityset_t} prior to any other use of that object.  If
such an object is not initialized in this way, but is nonetheless 
supplied as an argument to any of the @code{log_facilityaddset}, 
@code{logfacilitydelset}, @code{log_facilityismember} or 
@code{log_open} functions, the results are undefined.

@page
@subsection log_facilityismember - Manipulate log facility sets

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_facilityismember(
  const log_facility_set_t *set,
  log_facility_t            facilityno,
  const int                *member
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EFAULT
The @code{set} or @code{member} argument is an invalid pointer.

@item EINVAL
The @code{facilityno} argument is not a valid facility.

@end table

@subheading DESCRIPTION:

The @code{log_facilityismember} function tests whether the facility
specified by the value of the argument @code{facilityno} is a member
of the set pointed to by the argument @code{set}.  Upon successful
completion, the @code{log_facilityismember} function either returns
a value of one to the location specified by @code{member} if the 
specified facility is a member of the specified set or value of
zero to the location specified by @code{member} if the specified
facility is not a member of the specified set.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

Applications shall call either @code{log_facilityemptyset} or 
@code{log_facilityfillset} at least once for each object of type
@code{log_facilityset_t} prior to any other use of that object.  If
such an object is not initialized in this way, but is nonetheless 
supplied as an argument to any of the @code{log_facilityaddset}, 
@code{logfacilitydelset}, @code{log_facilityismember} or 
@code{log_open} functions, the results are undefined.

@page
@subsection log_create - Creates a log file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_create(
  logd_t       *ld,
  const char   *path,
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item ENOMEM
The is ????????????

@end table

@subheading DESCRIPTION:

This function dynamically allocates memory for the @code{ld}, associates 
a directory path to the @code{ld}, and provides access permissions to the
@code{ld}.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

@page
@subsection log_sys_create - Creates a system log file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_sys_create();
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EEXIST
The directory path to the system log already exist.

@end table

@subheading DESCRIPTION:

This function will create a predefined system log directory path and
system log file if they do not already exist.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.
