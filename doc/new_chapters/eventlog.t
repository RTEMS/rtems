@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Event Logging Manager

@section Introduction

The 
event logging manager is ...

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

@section Operations

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
The severity argument exceeds {LOG_SEVERITY_MAX}
@item EINVAL
The len argument exceeds {LOG_ENTRY_MAXLEN}
@item ENOSPC
The log file has run out of space on the device.
@item ENOSYS
The function log_write() is not supported by this implementation.
@item EPERM
The caller does not have appropriate permission for writing to
the given facility.
@item EIO
An I/O error occurred in writing to the system event log.

@end table

@subheading DESCRIPTION:

If {_POSIX_LOGGING} is defined:

   The @code{log_write} function writes an event record, consisting 
   of event attributes, and the data identified by the @code{buf} 
   argument, to the system log.  The @code{len} argument specifies
   the length in bytes of the buffer pointed to by @code{buf}.  The
   @code{len} argument shall specify the value of the event record
   length attribute.  The value of @code{len} shall be less than or 
   equal to {LOG_ENTRY_MAXLEN} or the @code{log_write} shall fail.

   The @code{event_id} argument identifies the type of event record
   being written.  The @code{event_id} argument shall specify the value
   of the event ID attribute of the event record.

   The argument @code{facility} indicates the facility from which the
   event type is drawn.  The @code{facility} aargument shall specify the
   value of the event record facility attribute.  The value of the
   @code{facility} argument shall be a valid log facility or the 
   @code{log_write} function shall fail.

   The @code{severity} argument indicates the severity level of the
   event record.  The @code{severity} argument shall specify the value
   of the event record severity attribute.  The value of the 
   @code{severity} argument shall be less than or equal to 
   {LOG_SEVERITY_MAX} or the @code{log_write} function shall fail.  

   The effective_UID of the calling process shall specify the event
   record UID attribute.  The efective-GID of the calling process 
   shall specify the event record GID attribute.  The process ID
   of the calling process shall specify the event record process ID
   attribute.  The process group ID of the calling process shall
   specify the event record process group ID attribute.  The current
   value of the system clock shall specify the event record timestamp
   attribute.

Otherwise:

   The @code{log_write} function shall fail.

@subheading NOTES:

@page
@subsection log_open - Open a log file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
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
{LOG_SEVERITY_MAX}.
@item EINVAL
The path argument referred to a file that was not a log file.
@item EMFILE
Too many log file descriptors are currently in use by this
process.
@item ENAMETOOLONG
The length of the path string exceeds {PATH_MAX}, or a pathname
component is longer than {NAME_MAX} while {_POSIX_NO_TRUNC} is
in effect.
@item ENFILE
Too many files are currently open in the system.
@item ENOENT
The file specified by the path argument does not exist.
@item ENOTDIR
A component of the path prefix is not a directory.
@item ENOSYS
The function log_open() is not supported by this implementation.

@end table

@subheading DESCRIPTION:

The @code{log_open} function establishes the connection between a 
log file and a log file descriptor. 

@subheading NOTES:

@page
@subsection log_read - Read from the system Log

@subheading CALLING SEQUENCE:

@ifset is-C
@example
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
@item ENOSYS
The function log_read() is not supported by this implementation.
@item EIO
An I/O error occurred in reading from the event log.

@end table

@subheading DESCRIPTION:


@subheading NOTES:

@page
@subsection log_notify - Notify Process of writes to the system log.

@subheading CALLING SEQUENCE:

@ifset is-C
@example
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
The logdes arfument is not a valid log file descriptor.
@item EINVAL
The notification argument specifies an invalid signal.
@item EINVAL
The process has requested a notify on a log that will not be
written to.
@item ENOSY
The function log_notify() is not supported by this implementation.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection log_close - Close log descriptor

@subheading CALLING SEQUENCE:

@ifset is-C
@example
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
@item ENOSYS
The function log_close() is not supported by t his implementation.

@end table

@subheading DESCRIPTION:

The @code{log_close} function deallocates the open log file
descriptor indicated by @code{log_des}.

@subheading NOTES:

@page
@subsection log_seek - Reposition log file offset

@subheading CALLING SEQUENCE:

@ifset is-C
@example
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
@item ENOSYS
The function log_seek() is not supported by this implementation.

@end table

@subheading DESCRIPTION:

The @code{log_seek} function shall set the log file offset of the open 
log descriptioin associated with the @code{logdes} log file descriptor to the
event record in the log file identified by @code{log_recid}.  

@subheading NOTES:

@page
@subsection log_severity_before - Compare event record severities

@subheading CALLING SEQUENCE:

@ifset is-C
@example
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
The value of either s1 or s2 exceeds {LOG_SEVERITY_MAX}.
@item ENOSYS
The function log_severity_before() is not supported by this 
implementation.

@end table

@subheading DESCRIPTION:

The @code{log_severity_before} function shall compare the severity order
of the @code{s1} and @code{s2} arguments. 

@subheading NOTES:

@page
@subsection log_facilityemptyset - Manipulate log facility sets

@subheading CALLING SEQUENCE:

@ifset is-C
@example
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
@item ENOSYS
The function is not supported by this implementation.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection log_facilityfillset - Manipulate log facility sets

@subheading CALLING SEQUENCE:

@ifset is-C
@example
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
@item ENOSYS
The function is not supported by this implementation.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection log_facilityaddset - Manipulate log facility sets

@subheading CALLING SEQUENCE:

@ifset is-C
@example
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
@item ENOSYS
The function is not supported by this implementation.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection log_facilitydelset - Manipulate log facility sets

@subheading CALLING SEQUENCE:

@ifset is-C
@example
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
@item ENOSYS
The function is not supported by this implementation.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection log_facilityismember - Manipulate log facility sets

@subheading CALLING SEQUENCE:

@ifset is-C
@example
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
@item ENOSYS
The function is not supported by this implementation.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

