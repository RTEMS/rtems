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
@item @code{log_write} - Write to the Log
@item @code{log_open} - Open a log file
@item @code{log_read} - Read from the system Log
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
@subsection log_write - Write to the Log

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
@item EINVAL
The facility argument is not a valid log_facility.

@item EINVAL
The severity argument exceeds @code{LOG_SEVERITY_MAX}

@item EINVAL
The len argument exceeds @code{LOG_ENTRY_MAXLEN}

@item ENOSPC
The log file has run out of space on the device.

@item EPERM
The caller does not have appropriate permission for writing to
the given facility.

@item EIO
An I/O error occurred in writing to the system event log.

@end table

@subheading DESCRIPTION:

The @code{log_write} function writes an event record, consisting 
of event attributes, and the data identified by the @code{buf} 
argument, to the system log.  The @code{len} argument specifies
the length in bytes of the buffer pointed to by @code{buf}.  The
@code{len} argument shall specify the value of the event record
length attribute.  The value of @code{len} shall be less than or 
equal to @code{LOG_ENTRY_MAXLEN} or the @code{log_write} shall fail.

The @code{event_id} argument identifies the type of event record
being written.  The @code{event_id} argument shall specify the value
of the event ID attribute of the event record.

The argument @code{facility} indicates the facility from which the
event type is drawn.  The @code{facility} argument shall specify the
value of the event record facility attribute.  The value of the
@code{facility} argument shall be a valid log facility or the 
@code{log_write} function shall fail.

The @code{severity} argument indicates the severity level of the
event record.  The @code{severity} argument shall specify the value
of the event record severity attribute.  The value of the 
@code{severity} argument shall be less than or equal to 
@code{LOG_SEVERITY_MAX} or the @code{log_write} function shall fail.  

The effective_UID of the calling process shall specify the event
record UID attribute.  The effective-GID of the calling process 
shall specify the event record GID attribute.  The process ID
of the calling process shall specify the event record process ID
attribute.  The process group ID of the calling process shall
specify the event record process group ID attribute.  The current
value of the system clock shall specify the event record timestamp
attribute.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

@page
@subsection log_open - Open a log file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_open(
  const logd_t         *logdes,
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
The log_facility field of the query argument is not a valid 
facility set.

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
description that refers to a log file and a log file descriptor that
refers to that open log file description.  The log file descriptor is
used by other log functions to refer to that log file.  The @code{path}
argument points to a pathname naming a log file.  A @code{path}
argument of NULL specifies the current system log file.  

The @code{query} argument points to a log query specification that
restricts log operations using the returned log file descriptor to 
to event records from the log file which match the query.  The 
predicate which determines the success of the match operation is the
logical AND of the individual comparison predicates for each member
of the log query specification.  The query attribute of the open file
description is set to filter as specified by the @code{query} argument.
If the value of the query argument is not NULL, the value of the 
@code{log_facility} member of the @code{query} specification shall be
a set of valid log facilities or the @code{log_open} shall fail.  If
the value of the @code{query} argument is not NULL, the value of the
@code{log_severity} member of the @code{query} specification shall be
less than or equal to @code{LOG_SEVERITY_MAX} or the @code{log_open}
shall fail.  If the value of the @code{query} argument is NULL, no
query filter shall be applied. 

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

@page
@subsection log_read - Read from the system Log

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_read(
  const logd_t logdes,
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
@item EBADF
The logdes argument is not a valid log file descriptor.

@item EBUSY
No data available.  The open log file descriptor references
the current system log.  and there are no unread event records
remaining.

@item EINTR
A signal interrupted the call to log_read().

@item EIO
An I/O error occurred in reading from the event log.

@end table

@subheading DESCRIPTION:

The @code{log_read} function shall attempt to read the @code{log_entry}
structure and @code{log_len} bytes of data from the next event record 
of the log file associated with the open log file descriptor @code{logdes},
placing the @code{log_entry} structure into the buffer pointed to by
@code{entry}, and the data into the buffer pointed to by @code{log_buf}.
The log record ID of the returned event record shall be stored in the 
@code{log_recid} member of the @code{log_entry} structure for the event
record.

If the query attribute of the open log file description associated with
the @code{logdes} is set, the event record read shall match that query.
If the @code{entry} argument is not NULL it will point to a @code{log_entry}
structure which shall be filled with the creation information for this log
entry.  If the argument @code{log_buf} is not NULL the data written with the
log entry will be placed in the buffer.  The size of the buffer is specified
by the argument @code{log_len}.

If the @code{log_read} is successful the call shall store the actual length
of the data associated with the event record into the location specified by
@code{log_sizeread}.  This number may be smaller or greater than 
@code{log_len}.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

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
have been closed, the open log file description shall be freed.

If the link count of the log file is zero, when all log file descriptors
have been closed, the space occupied by the log file shall be freed and the
log file shall no longer be accessible.

If the process has successfully registered a notification request for the 
log file descriptor, the registration shall be removed. 

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
The logdes argument is not a valid log file descriptor.

@item EINTR
The log_seek() function was interrupted by a signal.

@item EINVAL
The log_recid argument is not a valid record id.

@end table

@subheading DESCRIPTION:

The @code{log_seek} function shall set the log file offset of the open 
log description associated with the @code{logdes} log file descriptor 
to the event record in the log file identified by @code{log_recid}.  
The @code{log_recid} argument is either the record id of a valid event
record or one of the following values, as defined in the header <evlog.h>:

@table @b
@item LOG_SEEK_START
Set log file position to point at the first event
record in the log file

@item LOG_SEEK_END
Set log file position to point after the last event 
record in the log file

@end table

If the @code{log_seek} function is interrupted, the state of the open log
file description associated with @code{logdes} is unspecified.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

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
@item EINVAL 
The value of either s1 or s2 exceeds @code{LOG_SEVERITY_MAX}.

@end table

@subheading DESCRIPTION:

The @code{log_severity_before} function shall compare the severity order
of the @code{s1} and @code{s2} arguments.  Severity values ordered 
according to this function shall be according to decreasing severity.

If @code{s1} is ordered before or is equal to @code{s2} then the ordering
predicate shall return 1, otherwise the predicate shall return 0.  If 
either @code{s1} or @code{s2} specify invalid severity values, the return
value of @code{log_severity_before} is unspecified.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

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
@item EINVAL
The facilityno argument is not a valid facility.

@end table

@subheading DESCRIPTION:

The facilitysetops primitives manipulate sets of facilities.  They 
operate on data objects addressable by the application.

The @code{log_facilityemptyset} function initializes the facility
set pointed to by the argument @code{set}, such that all facilities
are included.

Applications shall call either @code{log_facilityemptyset} or 
@code{log_facilityfillset} at least once for each object of type
@code{log_facilityset_t} prior to any other use of that object.  If
such an object is not initialized in this way, but is nonetheless 
supplied as an argument  to any of the @code{log_facilityaddset}, 
@code{logfacilitydelset}, @code{log_facilityismember} or 
@code{log_open} functions, the results are undefined.

The @code{log_facilityaddset} and @code{log_facilitydelset} functions
respectively add or delete the individual facility specified by the
value of the argument @code{facilityno} to or from the facility set
pointed to by the argument @code{set}

The @code{log_facilityismember} function tests whether the facility
specified by the value of the argument @code{facilityno} is a member
of the set pointed to by the argument @code{set}.  Upon successful
completion, the @code{log_facilityismember} function either returns
a value of one to the location specified by @code{member} if the 
specified facility is a member of the specified set or returns a 
value of zero to the location specified by @code{member} if the
specified facility is not a member of the specified set.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

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
@item EINVAL
The facilityno argument is not a valid facility.

@end table

@subheading DESCRIPTION:

The facilitysetops primitives manipulate sets of facilities.  They 
operate on data objects addressable by the application.

The @code{log_facilityemptyset} function initializes the facility
set pointed to by the argument @code{set}, such that all facilities
are included.

Applications shall call either @code{log_facilityemptyset} or 
@code{log_facilityfillset} at least once for each object of type
@code{log_facilityset_t} prior to any other use of that object.  If
such an object is not initialized in this way, but is nonetheless 
supplied as an argument  to any of the @code{log_facilityaddset}, 
@code{logfacilitydelset}, @code{log_facilityismember} or 
@code{log_open} functions, the results are undefined.

The @code{log_facilityaddset} and @code{log_facilitydelset} functions
respectively add or delete the individual facility specified by the
value of the argument @code{facilityno} to or from the facility set
pointed to by the argument @code{set}

The @code{log_facilityismember} function tests whether the facility
specified by the value of the argument @code{facilityno} is a member
of the set pointed to by the argument @code{set}.  Upon successful
completion, the @code{log_facilityismember} function either returns
a value of one to the location specified by @code{member} if the 
specified facility is a member of the specified set or returns a 
value of zero to the location specified by @code{member} if the
specified facility is not a member of the specified set.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

@page
@subsection log_facilityaddset - Manipulate log facility sets

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_facilityaddset(
  log_facility_set_t  *set,
  log_facility_t      facilityno
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
The facilityno argument is not a valid facility.

@end table

@subheading DESCRIPTION:

The facilitysetops primitives manipulate sets of facilities.  They 
operate on data objects addressable by the application.

The @code{log_facilityemptyset} function initializes the facility
set pointed to by the argument @code{set}, such that all facilities
are included.

Applications shall call either @code{log_facilityemptyset} or 
@code{log_facilityfillset} at least once for each object of type
@code{log_facilityset_t} prior to any other use of that object.  If
such an object is not initialized in this way, but is nonetheless 
supplied as an argument  to any of the @code{log_facilityaddset}, 
@code{logfacilitydelset}, @code{log_facilityismember} or 
@code{log_open} functions, the results are undefined.

The @code{log_facilityaddset} and @code{log_facilitydelset} functions
respectively add or delete the individual facility specified by the
value of the argument @code{facilityno} to or from the facility set
pointed to by the argument @code{set}

The @code{log_facilityismember} function tests whether the facility
specified by the value of the argument @code{facilityno} is a member
of the set pointed to by the argument @code{set}.  Upon successful
completion, the @code{log_facilityismember} function either returns
a value of one to the location specified by @code{member} if the 
specified facility is a member of the specified set or returns a 
value of zero to the location specified by @code{member} if the
specified facility is not a member of the specified set.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

@page
@subsection log_facilitydelset - Manipulate log facility sets

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <evlog.h>

int log_facilitydelset(
  log_facility_set_t  *set,
  log_facility_t      facilityno
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
The facilityno argument is not a valid facility.

@end table

@subheading DESCRIPTION:

The facilitysetops primitives manipulate sets of facilities.  They 
operate on data objects addressable by the application.

The @code{log_facilityemptyset} function initializes the facility
set pointed to by the argument @code{set}, such that all facilities
are included.

Applications shall call either @code{log_facilityemptyset} or 
@code{log_facilityfillset} at least once for each object of type
@code{log_facilityset_t} prior to any other use of that object.  If
such an object is not initialized in this way, but is nonetheless 
supplied as an argument  to any of the @code{log_facilityaddset}, 
@code{logfacilitydelset}, @code{log_facilityismember} or 
@code{log_open} functions, the results are undefined.

The @code{log_facilityaddset} and @code{log_facilitydelset} functions
respectively add or delete the individual facility specified by the
value of the argument @code{facilityno} to or from the facility set
pointed to by the argument @code{set}

The @code{log_facilityismember} function tests whether the facility
specified by the value of the argument @code{facilityno} is a member
of the set pointed to by the argument @code{set}.  Upon successful
completion, the @code{log_facilityismember} function either returns
a value of one to the location specified by @code{member} if the 
specified facility is a member of the specified set or returns a 
value of zero to the location specified by @code{member} if the
specified facility is not a member of the specified set.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

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
@item EINVAL
The facilityno argument is not a valid facility.

@end table

@subheading DESCRIPTION:

The facilitysetops primitives manipulate sets of facilities.  They 
operate on data objects addressable by the application.

The @code{log_facilityemptyset} function initializes the facility
set pointed to by the argument @code{set}, such that all facilities
are included.

Applications shall call either @code{log_facilityemptyset} or 
@code{log_facilityfillset} at least once for each object of type
@code{log_facilityset_t} prior to any other use of that object.  If
such an object is not initialized in this way, but is nonetheless 
supplied as an argument  to any of the @code{log_facilityaddset}, 
@code{logfacilitydelset}, @code{log_facilityismember} or 
@code{log_open} functions, the results are undefined.

The @code{log_facilityaddset} and @code{log_facilitydelset} functions
respectively add or delete the individual facility specified by the
value of the argument @code{facilityno} to or from the facility set
pointed to by the argument @code{set}

The @code{log_facilityismember} function tests whether the facility
specified by the value of the argument @code{facilityno} is a member
of the set pointed to by the argument @code{set}.  Upon successful
completion, the @code{log_facilityismember} function either returns
a value of one to the location specified by @code{member} if the 
specified facility is a member of the specified set or returns a 
value of zero to the location specified by @code{member} if the
specified facility is not a member of the specified set.

@subheading NOTES:

The @code{_POSIX_LOGGING} feature flag is defined to indicate
this service is available.

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
