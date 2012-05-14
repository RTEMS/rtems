@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Process Environment Manager

@section Introduction

The process environment manager is responsible for providing the
functions related to user and group Id management.

The directives provided by the process environment manager are:

@itemize @bullet
@item @code{getpid} - Get Process ID
@item @code{getppid} - Get Parent Process ID
@item @code{getuid} - Get User ID
@item @code{geteuid} - Get Effective User ID
@item @code{getgid} - Get Real Group ID
@item @code{getegid} - Get Effective Group ID
@item @code{setuid} - Set User ID
@item @code{setgid} - Set Group ID
@item @code{getgroups} - Get Supplementary Group IDs
@item @code{getlogin} - Get User Name
@item @code{getlogin_r} - Reentrant Get User Name
@item @code{getpgrp} - Get Process Group ID
@item @code{setsid} - Create Session and Set Process Group ID
@item @code{setpgid} - Set Process Group ID for Job Control
@item @code{uname} - Get System Name
@item @code{times} - Get Process Times
@item @code{getenv} - Get Environment Variables
@item @code{setenv} - Set Environment Variables
@item @code{ctermid} - Generate Terminal Pathname
@item @code{ttyname} - Determine Terminal Device Name
@item @code{ttyname_r} - Reentrant Determine Terminal Device Name
@item @code{isatty} - Determine if File Descriptor is Terminal
@item @code{sysconf} - Get Configurable System Variables
@end itemize

@section Background

@subsection Users and Groups

RTEMS provides a single process, multi-threaded execution environment.
In this light, the notion of user and group is somewhat without meaning.
But RTEMS does provide services to provide a synthetic version of
user and group.  By default, a single user and group is associated 
with the application.  Thus unless special actions are taken,
every thread in the application shares the same user and group Id.
The initial rationale for providing user and group Id functionality
in RTEMS was for the filesystem infrastructure to implement
file permission checks.  The effective user/group Id capability
has since been used to implement permissions checking by
the @code{ftpd} server.

In addition to the "real" user and group Ids, a process may
have an effective user/group Id.  This allows a process to
function using a more limited permission set for certain operations.

@subsection User and Group Names

POSIX considers user and group Ids to be a unique integer that
may be associated with a name.  This is usually accomplished
via a file named @code{/etc/passwd} for user Id mapping and
@code{/etc/groups} for group Id mapping.  Again, although
RTEMS is effectively a single process and thus single user
system, it provides limited support for user and group
names.  When configured with an appropriate filesystem, RTEMS 
will access the appropriate files to map user and group Ids
to names.

If these files do not exist, then RTEMS will synthesize
a minimal version so this family of services return without
error.  It is important to remember that a design goal of
the RTEMS POSIX services is to provide useable and 
meaningful results even though a full process model
is not available.

@subsection Environment Variables

POSIX allows for variables in the run-time environment.  These are 
name/value pairs that make be dynamically set and obtained by
programs.  In a full POSIX environment with command line shell
and multiple processes,  environment variables may be set in
one process -- such as the shell -- and inherited by child
processes.  In RTEMS, there is only one process and thus
only one set of environment variables across all processes.


@section Operations

@subsection Accessing User and Group Ids

The user Id associated with the current thread may be obtain
using the @code{getuid()} service.  Similarly, the group Id
may be obtained using the @code{getgid()} service.  

@subsection Accessing Environment Variables

The value associated with an environment variable may be 
obtained using the @code{getenv()} service and set using
the @code{putenv()} service.

@section Directives

This section details the process environment manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection getpid - Get Process ID

@findex getpid
@cindex  get process id

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getpid( void );
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

The process Id is returned.

@subheading DESCRIPTION:

This service returns the process Id.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection getppid - Get Parent Process ID

@findex getppid
@cindex  get parent process id

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getppid( void );
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

The parent process Id is returned.

@subheading DESCRIPTION:

This service returns the parent process Id.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection getuid - Get User ID

@findex getuid
@cindex  get user id

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getuid( void );
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

The effective user Id is returned.

@subheading DESCRIPTION:

This service returns the effective user Id.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection geteuid - Get Effective User ID

@findex geteuid
@cindex  get effective user id

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int geteuid( void );
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

The effective group Id is returned.

@subheading DESCRIPTION:

This service returns the effective group Id.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection getgid - Get Real Group ID

@findex getgid
@cindex  get real group id

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getgid( void );
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

The group Id is returned.

@subheading DESCRIPTION:

This service returns the group Id.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection getegid - Get Effective Group ID

@findex getegid
@cindex  get effective group id

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getegid( void );
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

The effective group Id is returned.

@subheading DESCRIPTION:

This service returns the effective group Id.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection setuid - Set User ID

@findex setuid
@cindex  set user id

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int setuid(
  uid_t uid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

This service returns 0.

@subheading DESCRIPTION:

This service sets the user Id to @code{uid}.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection setgid - Set Group ID

@findex setgid
@cindex  set group id

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int setgid(
  gid_t  gid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

This service returns 0.

@subheading DESCRIPTION:

This service sets the group Id to @code{gid}.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection getgroups - Get Supplementary Group IDs

@findex getgroups
@cindex  get supplementary group ids

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getgroups(
  int    gidsetsize,
  gid_t  grouplist[]
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

NA

@subheading DESCRIPTION:

This service is not implemented as RTEMS has no notion of 
supplemental groups.

@subheading NOTES:

If supported, this routine would only be allowed for
the super-user.

@c
@c
@c
@page
@subsection getlogin - Get User Name

@findex getlogin
@cindex  get user name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
char *getlogin( void );
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

Returns a pointer to a string containing the name of the
current user.

@subheading DESCRIPTION:

This routine returns the name of the current user.

@subheading NOTES:

This routine is not reentrant and subsequent calls to
@code{getlogin()} will overwrite the same buffer.

@c
@c
@c
@page
@subsection getlogin_r - Reentrant Get User Name

@findex getlogin_r
@cindex  reentrant get user name
@cindex  get user name, reentrant

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getlogin_r(
  char   *name,
  size_t  namesize  
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
The arguments were invalid.

@end table

@subheading DESCRIPTION:

This is a reentrant version of the @code{getlogin()} service.  The 
caller specified their own buffer, @code{name}, as well as the 
length of this buffer, @code{namesize}.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection getpgrp - Get Process Group ID

@findex getpgrp
@cindex  get process group id

@subheading CALLING SEQUENCE:

@ifset is-C
@example
pid_t getpgrp( void );
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

The procress group Id is returned.

@subheading DESCRIPTION:

This service returns the current progress group Id.

@subheading NOTES:

This routine is implemented in a somewhat meaningful
way for RTEMS but is truly not functional.

@c
@c
@c
@page
@subsection setsid - Create Session and Set Process Group ID

@findex setsid
@cindex  create session and set process group id

@subheading CALLING SEQUENCE:

@ifset is-C
@example
pid_t setsid( void );
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EPERM
The application does not have permission to create a process group.

@end table

@subheading DESCRIPTION:

This routine always returns @code{EPERM} as RTEMS has no way
to create new processes and thus no way to create a new process
group.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection setpgid - Set Process Group ID for Job Control

@findex setpgid
@cindex  set process group id for job control

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int setpgid(
  pid_t pid,
  pid_t pgid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item ENOSYS
The routine is not implemented.

@end table

@subheading DESCRIPTION:

This service is not implemented for RTEMS as process groups are not
supported.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection uname - Get System Name

@findex uname
@cindex  get system name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int uname(
  struct utsname *name
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EPERM
The provided structure pointer is invalid.

@end table

@subheading DESCRIPTION:

This service returns system information to the caller.  It does this
by filling in the @code{struct utsname} format structure for the
caller.

@subheading NOTES:

The information provided includes the operating system (RTEMS in
all configurations), the node number, the release as the RTEMS
version, and the CPU family and model.  The CPU model name 
will indicate the multilib executive variant being used.

@c
@c
@c
@page
@subsection times - Get process times

@findex times
@cindex  get process times

@subheading CALLING SEQUENCE:

@example
#include <sys/time.h>

clock_t times(
  struct tms *ptms
);
@end example

@subheading STATUS CODES:

This routine returns the number of clock ticks that have elapsed
since the system was initialized (e.g. the application was 
started).

@subheading DESCRIPTION:

@code{times} stores the current process times in @code{ptms}.  The
format of @code{struct tms} is as defined in
@code{<sys/times.h>}.  RTEMS fills in the field @code{tms_utime}
with the number of ticks that the calling thread has executed
and the field @code{tms_stime} with the number of clock ticks
since system boot (also returned).  All other fields in the
@code{ptms} are left zero.

@subheading NOTES:

RTEMS has no way to distinguish between user and system time
so this routine returns the most meaningful information 
possible.

@c
@c
@c
@page
@subsection getenv - Get Environment Variables

@findex getenv
@cindex  get environment variables

@subheading CALLING SEQUENCE:

@ifset is-C
@example
char *getenv(
  const char *name
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item NULL
when no match

@item pointer to value
when successful

@end table

@subheading DESCRIPTION:

This service searches the set of environment variables for 
a string that matches the specified @code{name}.  If found,
it returns the associated value.

@subheading NOTES:

The environment list consists of name value pairs that
are of the form @i{name = value}.

@c
@c
@c
@page
@subsection setenv - Set Environment Variables

@findex setenv
@cindex  set environment variables

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int setenv(
  const char *name,
  const char *value,
  int overwrite
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

Returns 0 if successful and -1 otherwise.

@subheading DESCRIPTION:

This service adds the variable @code{name} to the environment with
@code{value}.  If @code{name} is not already exist, then it is
created.  If @code{name} exists and @code{overwrite} is zero, then
the previous value is not overwritten. 

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection ctermid - Generate Terminal Pathname

@findex ctermid
@cindex  generate terminal pathname

@subheading CALLING SEQUENCE:

@ifset is-C
@example
char *ctermid(
  char *s
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

Returns a pointer to a string indicating the pathname for the controlling
terminal.

@subheading DESCRIPTION:

This service returns the name of the terminal device associated with
this process.  If @code{s} is NULL, then a pointer to a static buffer
is returned.  Otherwise, @code{s} is assumed to have a buffer of
sufficient size to contain the name of the controlling terminal.

@subheading NOTES:

By default on RTEMS systems, the controlling terminal is @code{/dev/console}.
Again this implementation is of limited meaning, but it provides
true and useful results which should be sufficient to ease porting
applications from a full POSIX implementation to the reduced
profile supported by RTEMS.

@c
@c
@c
@page
@subsection ttyname - Determine Terminal Device Name

@findex ttyname
@cindex  determine terminal device name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
char *ttyname(
  int fd
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

Pointer to a string containing the terminal device name or
NULL is returned on any error.

@subheading DESCRIPTION:

This service returns a pointer to the pathname of the terminal
device that is open on the file descriptor @code{fd}.  If 
@code{fd} is not a valid descriptor for a terminal device,
then NULL is returned.

@subheading NOTES:

This routine uses a static buffer.

@c
@c
@c
@page
@subsection ttyname_r - Reentrant Determine Terminal Device Name

@findex ttyname_r
@cindex  reentrant determine terminal device name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ttyname_r(
  int   fd,
  char *name,
  int   namesize
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

This routine returns -1 and sets @code{errno} as follows:

@table @b
@item EBADF
If not a valid descriptor for a terminal device.

@item EINVAL
If @code{name} is NULL or @code{namesize} are insufficient.

@end table

@subheading DESCRIPTION:

This service the pathname of the terminal device that is open
on the file descriptor @code{fd}.  

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection isatty - Determine if File Descriptor is Terminal

@findex isatty
@cindex  determine if file descriptor is terminal

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int isatty(
  int fd
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

Returns 1 if @code{fd} is a terminal device and 0 otherwise.

@subheading DESCRIPTION:

This service returns 1 if @code{fd} is an open file descriptor 
connected to a terminal and 0 otherwise.

@subheading NOTES:

@c
@c
@c
@page
@subsection sysconf - Get Configurable System Variables

@findex sysconf
@cindex  get configurable system variables

@subheading CALLING SEQUENCE:

@ifset is-C
@example
long sysconf(
  int name
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

The value returned is the actual value of the system resource.
If the requested configuration name is a feature flag, then
1 is returned if the available and 0 if it is not.  On any
other error condition, -1 is returned.

@subheading DESCRIPTION:

This service is the mechanism by which an application determines
values for system limits or options at runtime. 

@subheading NOTES:

Much of the information that may be obtained via @code{sysconf}
has equivalent macros in @code{<unistd.h}.  However, those
macros reflect conservative limits which may have been altered
by application configuration.
