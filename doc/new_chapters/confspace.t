@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Configuration Space Manager

@section Introduction

The 
configuration space manager is ...

The directives provided by the configuration space manager are:

@itemize @bullet
@item @code{cfg_mount} - Mount a Configuration Space
@item @code{cfg_unmount} - 
@item @code{cfg_mknod} - 
@item @code{cfg_get} - 
@item @code{cfg_set} - 
@item @code{cfg_link} - 
@item @code{cfg_unlink} - 
@item @code{cfg_open} - 
@item @code{cfg_read} - 
@item @code{cfg_children} - 
@item @code{cfg_mark} - 
@item @code{cfg_close} - 
@end itemize

@section Background

@section Operations

@section Directives

This section details the configuration space manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection cfg_mount - Mount a Configuration Space

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_mount(
  const char     *file,
  const char     *cfgpath,
  log_facility_t  notification,
  int             logdes
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EPERM
The caller does not have the appropriate privilege.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_unmount - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_unmount(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_mknod - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_mknod(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_get - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_get(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_set - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_set(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_link - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_link(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_unlink - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_unlink(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_open - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_open(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_read - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_read(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_children - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_children(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_mark - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_mark(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_close - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_close(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

