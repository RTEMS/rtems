@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Configuration Space Manager

@section Introduction

The configuration space manager provides a portable
interface for manipulating configuration data.
The capabilities in this manager were defined in the POSIX
1003.1h/D3 proposed standard titled @b{Services for Reliable,
Available, and Serviceable Systems}.

The directives provided by the configuration space manager are:

@itemize @bullet
@item @code{cfg_mount} - Mount a Configuration Space
@item @code{cfg_unmount} - Unmount a Configuration Space
@item @code{cfg_mknod} - Create a Configuration Node
@item @code{cfg_get} - Get Configuration Node Value
@item @code{cfg_set} - Set Configuration Node Value
@item @code{cfg_link} - Create a Configuration Link
@item @code{cfg_unlink} - Remove a Configuration Link
@item @code{cfg_open} - Open a Configuration Space
@item @code{cfg_read} - Read a Configuration Space
@item @code{cfg_children} - Get Node Entries
@item @code{cfg_mark} - Set Configuration Space Option
@item @code{cfg_readdir} - Reads a directory
@item @code{cfg_umask} - Sets a file creation mask
@item @code{cfg_chmod} - Changes file mode
@item @code{cfg_chown} - Changes the owner and/or group of a file
@end itemize

@section Background

@subsection Configuration Nodes

@subsection Configuration Space

@subsection Format of a Configuration Space File

@section Operations

@subsection Mount and Unmounting

@subsection Creating a Configuration Node

@subsection Removing a Configuration Node

@subsection Manipulating a Configuration Node

@subsection Traversing a Configuration Space

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
#include <cfg.h>

int cfg_mount(
  const char     *file,
  const char     *cfgpath,
  log_facility_t  notification,
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_mount()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item EPERM
The caller does not have the appropriate privilege.

@item EACCES
Search permission is denied for a component of the path prefix.

@item EEXIST
The file specified by the @code{file} argument does not exist

@item ENAMETOOLONG
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters while
@code{_POSIX_NO_TRUNC} is in effect.

@item ENOENT
A component of @code{cfgpath} does not exist.

@item ENOTDIR
A component of the @code{file} path prefix is not a directory.

@item EBUSY
The configuration space defined by @code{file} is already mounted.

@item EINVAL
The notification argument specifies an invalid log facility.

@end table

@subheading DESCRIPTION:

The @code{cfg_mount()} function maps a configuration space defined
by the file identified by the the @code{file} argument.  The 
distinguished node of the mapped configuration space is
mounted in the active space at the point identified by the
@code{cfgpath} configuration pathname.

The @code{notification} argument specifies how changes to the
mapped configuration space are communicated to the application.
If the @code{notification} argument is NULL, no notification will be
be performed for the mapped configuration space.  If the Event
Logging option is defined, the notification argument defines the
facility to which changes in the mapped configuration space are
logged.  Otherwise, the @code{notification} argument specifies
an implementation defined method of notifying the application
of changes to the mapped configuration space.

@subheading NOTES:

The @code{_POSIX_CFG} feature flag is defined to indicate
this service is available.

@page
@subsection cfg_unmount - Unmount a Configuration Space

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <cfg.h>

int cfg_unmount(
  const char     *cfgpath
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_umount()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item EPERM
The caller does not have the appropriate privileges.

@item EACCES
Search permission is denied for a component of the path prefix.

@item ENOENT
A component of @code{cfgpath} does not exist.

@item ENAMETOOLONG
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters while
@code{_POSIX_NO_TRUNC} is in effect.

@item EINVAL
The requested node is not the distinguished node of a mounted
configuration space.

@item EBUSY
One or more processes has an open configuration traversal
stream for the configuration space whose distinguished node is
referenced by the cfgpath argument.

@item ELOOP
A node appears more than once in the path specified by the 
@code{cfgpath} argument

@item ELOOP
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument

@end table

@subheading DESCRIPTION:

The @code{cfg_umount()} function unmaps the configuration space whose 
distinguished node is mapped in the active space at the location defined
by @code{cfgpath} configuration pathname.  All system resources 
allocated for this configuration space should be deallocated.

@subheading NOTES:

The @code{_POSIX_CFG} feature flag is defined to indicate
this service is available.

@page
@subsection cfg_mknod - Create a Configuration Node

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <cfg.h>

int cfg_mknod(
  const char   *cfgpath,
  mode_t        mode,
  cfg_type_t    type         
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_mknod()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item ENAMETOOLONG
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters while
@code{_POSIX_NO_TRUNC} is in effect.

@item ENOENT
A component of the path prefix does not exist.

@item EACCES
Search permission is denied for a component of the path prefix.


@item EPERM
The calling process does not have the appropriate privilege.

@item EEXIST
The named node exists.

@item EINVAL
The value of @code{mode} is invalid.

@item EINVAL
The value of @code{type} is invalid.

@item ELOOP
A node appears more than once in the path specified by the 
@code{cfg_path} argument

@item ELOOP
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the @code{cfgpath} argument.

@item EROFS
The named @code{node} resides on a read-only configuration space.

@end table

@subheading DESCRIPTION:

The @code{cfg_mknod()} function creates a new node in the configuration
space which contains the pathname prefix of @code{cfgpath}.  The node
name is defined by the pathname suffix of @code{cfgpath}. The node
permissions are specified by the value of @code{mode}.  The node type
is specified by the value of @code{type}.

@subheading NOTES:

The @code{_POSIX_CFG} feature flag is defined to indicate
this service is available.

@page
@subsection cfg_get - Get Configuration Node Value

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <cfg.h>

int cfg_get(
  const char  *cfgpath
  cfg_value_t *value
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_get()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item ENAMETOOLONG
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters while
@code{_POSIX_NO_TRUNC} is in effect.

@item ENOENT
A component of @code{cfgpath} does not exist.

@item EACCES
Search permission is denied for a component of the path prefix.

@item EPERM
The calling process does not have the appropriate privileges.

@item ELOOP
A node appears more than once in the path specified by the 
@code{cfgpath} argument

@item ELOOP
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the @code{cfgpath} argument.

@end table

@subheading DESCRIPTION:

The @code{cfg_get()} function stores the value attribute of the
configuration node identified by @code{cfgpath}, into the buffer
described by the @code{value} pointer.

@subheading NOTES:

The @code{_POSIX_CFG} feature flag is defined to indicate
this service is available.

@page
@subsection cfg_set - Set Configuration Node Value

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <cfg.h>

int cfg_set(
  const char  *cfgpath
  cfg_value_t *value
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_set()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item ENAMETOOLONG
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters while
@code{_POSIX_NO_TRUNC} is in effect.

@item ENOENT
A component of @code{cfgpath} does not exist

@item EACCES
Search permission is denied for a component of the path prefix.

@item EPERM
The calling process does not have the appropriate privilege.

@item ELOOP
A node appears more than once in the path specified by the
@code{cfgpath} argument.

@item ELOOP
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.

@end table

@subheading DESCRIPTION:

The @code{cfg_set()} function stores the value specified by the
@code{value} argument in the configuration node defined by the 
@code{cfgpath} argument.

@subheading NOTES:

The @code{_POSIX_CFG} feature flag is defined to indicate
this service is available.

@page
@subsection cfg_link - Create a Configuration Link

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <cfg.h>

int cfg_link(
  const char *src
  const char *dest
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_link()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item ENAMETOOLONG
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters while
@code{_POSIX_NO_TRUNC} is in effect.

@item ENOENT
A component of either path prefix does not exist.

@item EACCES
A component of either path prefix denies search permission.

@item EACCES
The requested link requires writing in a node with a mode that
denies write permission.

@item ENOENT
The node named by @code{src} does not exist.

@item EEXIST
The node named by @code{dest} does exist.

@item EPERM
The calling process does not have the appropriate privilege to
modify the node indicated by the @code{src} argument.

@item EXDEV
The link named by @code{dest} and the node named by @code{src} are from different
configuration spaces.

@item ENOSPC
The node in which the entry for the new link is being placed
cannot be extended because there is no space left on the 
configuration space containing the node.

@item EIO
An I/O error occurred while reading from or writing to the 
configuration space to make the link entry.

@item EROFS
The requested link requires writing in a node on a read-only
configuration space.

@end table

@subheading DESCRIPTION:

The @code{src} and @code{dest} arguments point to pathnames which 
name existing nodes.  The @code{cfg_link()} function atomically creates
a link between specified nodes, and increment by one the link count 
of the node specified by the @code{src} argument. 

If the @code{cfg_link()} function fails, no link is created, and the
link count of the node remains unchanged by this function call.


@subheading NOTES:

The @code{_POSIX_CFG} feature flag is defined to indicate
this service is available.

@page
@subsection cfg_unlink - Remove a Configuration Link

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <cfg.h>

int cfg_unlink(
  const char    *cfgpath
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_unlink()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item ENAMETOOLONG
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters.

@item EACCES
Search permission is denied on the node containing the link to
be removed.

@item EACCES
Write permission is denied on the node containing the link to 
be removed.

@item ENOENT
A component of @code{cfgpath} does not exist.

@item EPERM
The calling process does not have the appropriate privilege to 
modify the node indicated by the path prefix of the @code{cfgpath}
argument.

@item EBUSY
The node to be unlinked is the distinguished node of a mounted
configuration space.

@item EIO
An I/O error occurred while deleting the link entry or deallocating
the node.

@item EROFS
The named node resides in a read-only configuration space.

@item ELOOP
A node appears more than once in the path specified by the
@code{cfgpath} argument.

@item ELOOP
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.

@end table

@subheading DESCRIPTION:

The @code{cfg_unlink()} function removes the link between the node
specified by the @code{cfgpath} path prefix and the parent node 
specified by @code{cfgpath}, and decrements the link count 
of the @code{cfgpath} node.

When the link count of the node becomes zero, the space occupied
by the node is freed and the node is no longer be accessible.

@subheading NOTES:

The @code{_POSIX_CFG} feature flag is defined to indicate
this service is available.

@page
@subsection cfg_open - Open a Configuration Space

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <cfg.h>

int cfg_open(
  const char     *pathnames[],
  int             options,
  int           (*compar)(const CFGENT **f1, const CFGENT **f2),
  CFG           **cfgstream
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_open()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item EACCES
Search permission is denied for any component of a pathname.

@item ELOOP
A loop exists in symbolic links encountered during resolution 
of a pathname.

@item ENAMETOOLONG
The length of a pathname exceeds @code{PATH_MAX}, or a pathname
component is longer than @code{NAME_MAX} while @code{_POSIX_NO_TRUNC}

@item ENOENT
The pathname argument is an empty string or the named node
does not exist.

@item EINVAL
Either both or neither of @code{CFG_LOGICAL} and @code{CFG_PHYSICAL} are
specified by the @code{options} argument

@item ENOMEM
Not enough memory is available to create the necessary structures.

@item ELOOP
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the @code{pathnames} argument.

@item ENAMETOOLONG
As a result of encountering a symbolic link in resolution of the
pathname specified by the @code{pathnames} argument, the length of
the substituted pathname string exceeded @code{PATH_MAX}.

@end table

@subheading DESCRIPTION:

The @code{cfg_open()} function opens a configuration traversal stream
rooted in the configuration nodes name by the @code{pathnames} argument.
It stores a pointer to a CFG object that represents that stream at 
the location identified the @code{cfgstream} pointer.  The @code{pathnames}
argument is an array of character pointers to NULL-terminated strings. 
The last member of this array is a NULL pointer.

The value of @code{options} is the bitwise inclusive OR of values from the 
following lists.  Applications supply exactly one of the first two values
below in @code{options}.

@table @b

@item CFG_LOGICAL
When symbolic links referencing existing nodes are 
encountered during the traversal, the @code{cfg_info}
field of the returned CFGENT structure describes the
target node pointed to by the link instead of the 
link itself, unless the target node does not exist. 
If the target node has children, the pre-order return,
followed by the return of structures referencing all of
its descendants, followed by a post-order return, is done.
                    
@item CFG_PHYSICAL
When symbolic links are encountered during the traversal,
the @code{cfg_info} field is used to describe the symbolic 
link.

@end table
                    

Any combination of the remaining flags can be specified in the value of 
@code{options}

@table @b

@item CFG_COMFOLLOW
When symbolic links referencing existing nodes are
specified in the @code{pathnames} argument, the 
@code{cfg_info} field of the returned CFGENT structure
describes the target node pointed to by the link
instead of the link itself, unless the target node does
not exist.  If the target node has children, the 
pre-order return, followed by the return of structures
referencing all its descendants, followed by a post-order
return, is done.

@item CFG_XDEV
The configuration space functions do not return a
CFGENT structure for any node in a different configuration
space than the configuration space of the nodes identified 
by the CFGENT structures for the @code{pathnames} argument.

@end table

The @code{cfg_open()} argument @code{compar} is either a NULL or point
to a function that is called with two pointers to pointers to CFGENT 
structures that returns less than, equal to , or greater than zero if 
the node referenced by the first argument is considered to be respectively
less than, equal to, or greater than the node referenced by the second.
The CFGENT structure fields provided to the comparison routine is as 
described with the exception that the contents of the @code{cfg_path} and
@code{cfg_pathlen} fields are unspecified.

This comparison routine is used to determine the order in which nodes in
directories encountered during the traversal are returned, and the order
of traversal when more than one node is specified in the @code{pathnames}
argument to @code{cfg_open()}.  If a comparison routine is specified, the
order of traversal is from the least to the greatest.  If the @code{compar}
argument is NULL, the order of traversal shall is listed in the 
@code{pathnames} argument. 

@subheading NOTES:

The @code{_POSIX_CFG} feature flag is defined to indicate
this service is available.

@page
@subsection cfg_read - Read a Configuration Space

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <cfg.h>

int cfg_read(  
  CFG           *cfgp,
  CFGENT       **node
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_read()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item EACCES
Search permission is denied for any component of a pathname.

@item EBADF
The @code{cfgp} argument does not refer to an open configuration
space.

@item ELOOP
A loop exists in symbolic links encountered during resolution
of a pathname.

@item ENOENT
A named @code{node} does not exist.

@item ENOMEM
Not enough memory is available to create the necessary structures.

@item ELOOP
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.

@item ENAMETOOLONG
As a result of encountering a symbolic link in resolution of the
pathname specified by the pathnames argument, the length of the
substituted pathname string exceeded @code{PATH_MATH}.

@end table

@subheading DESCRIPTION:

The @code{cfg_read()} function returns a pointer to a CFGENT structure
representing a node in the configuration space to which @code{cfgp}
refers.  The returned pointer is stored at the location indicated 
by the @code{node} argument.

The child nodes of each node in the configuration tree is returned
by @code{cfg_read()}.  If a comparison routine was specified to the
@code{cfg_open()} function, the order of return of the child nodes is
as specified by the @code{compar} routine, from least to greatest.  
Otherwise, the order of return is unspecified.

Structures referencing nodes with children is returned by the 
function @code{cfg_read()} at least twice [unless the application
specifies otherwise with @code{cfg_mark()}]-once immediately before
the structures representing their descendants, are returned 
(pre-order), and once immediately after structures representing all
of their descendants, if any, are returned (post-order).  The 
CFGENT structure returned in post-order (with the exception of the 
@code{cfg_info} field) is identical to that returned in pre-order.
Structures referencing nodes of other types is returned at least
once.

The fields of the CFGENT structure contains the following 
information:

@table @b

@item cfg_parent
A pointer to the structure returned by the 
@code{cfg_read()} function for the node that contains
the entry for the current node.  A @code{cfg_parent}
structure is provided for the node(s) specified by
the @code{pathnames} argument to the @code{cfg_open()}
function, but the contents of other than its 
@code{cfg_number}, @code{cfg_pointer}, @code{cfg_parent},
and @code{cfg_parent}, and @code{cfg_level} fields are
unspecified.  Its @code{cfg_link} field is unspecified.

@item cfg_link
Upon return from the @code{cfg_children()} function, the
@code{cfg_link} field points to the next CFGENT structure 
in a NULL-terminated linked list of CFGENT structures.  
Otherwise, the content of the @code{cfg_link} field is 
unspecified.

@item cfg_cycle
If the structure being returned by @code{cfg_read()} 
represents a node that appears in the @code{cfg_parent}
linked list tree, the @code{cfg_cycle} field shall point 
to the structure representing that entry from the 
@code{cfg_parent} linked list.  Otherwise the content of
the @code{cfg_cycle} field is unspecified.

@item cfg_number
The @code{cfg_number} field is provided for use by the 
application program.  It is initialized to zero for 
each new node returned by the @code{cfg_read()} function, 
but is not further modified by the configuration space
routines.

@item cfg_pointer
The @code{cfg_pointer} field is provided for use by the
application program.  It is initialized to NULL for
each new node returned by the @code{cfg_read()} function, 
but is not further modified by the configuration 
space routines.

@item cfg_path
A pathname for the node including and relative to the 
argument supplied to the @code{cfg_open()} routine for this
configuration space.  This pathname may be longer than
@code{PATH_MAX} bytes.  This pathname is NULL-terminated.

@item cfg_name
The nodename of the node.

@item cfg_pathlen
The length of the string pointed at by the @code{cfg_path}
field when returned by @code{cfg_read()}.

@item cfg_namelen
The length of the string pointed at by the @code{cfg_name}
field.

@item cfg_level
The depth of the current entry in the configuration space.
The @code{cfg_level} field of the @code{cfg_parent} 
structure for each of the node(s) specified in the 
@code{pathnames} argument to the @code{cfg_open()} function
is set to 0, and this number is incremented for each
node level descendant.

@item cfg_info
This field contains one of the values listed below.  If
an object can have more than one info value, the first 
appropriate value listed below is returned. 

@table @b

@item CFG_D
The structure represents a node with children in
pre-order.

@item CFG_DC
The structure represents a node that is a parent
of the node most recently returned by @code{cfg_read()}.
The @code{cfg_cycle} field references the structure 
previously returned by @code{cfg_read} that is the
same as the returned structure.

@item CFG_DEFAULT
The structure represents a node that is not 
represented by one of the other node types

@item CFG_DNR
The structure represents a node, not of type symlink,
that is unreadable.  The variable @code{cfg_errno}
is set to the appropriate value.

@item CFG_DP
The structure represents a node with children in
post-order.  This value occurs only if CFG_D 
has previously been returned for this entry.

@item CFG_ERR
The structure represents a node for which an error has 
occurred.  The variable @code{cfg_errno} is set to the
appropriate value.

@item CFG_F
The structure represents a node without children.

@item CFG_SL
The structure represents a node of type symbolic link.

@item CFG_SLNONET
The structure represents a node of type symbolic link
with a target node for which node characteristic
information cannot be obtained.

@end table

@end table

Structures returned by @code{cfg_read()} with a @code{cfg_info} field equal
to CFG_D is accessible until a subsequent call, on the same
configuration traversal stream, to @code{cfg_close()}, or to @code{cfg_read()}
after they have been returned by the @code{cfg_read} function in
post-order.  Structures returned by @code{cfg_read()} with an
@code{cfg_info} field not equal to CFG_D is accessible until a subsequent
call, on the same configuration traversal stream, to @code{cfg_close()}
or @code{cfg_read()}. 

The content of the @code{cfg_path} field is specified only for the
structure most recently returned by @code{cfg_read()}. 

The specified fields in structures in the list representing nodes for
which structures have previously been returned by @code{cfg_children()},
is identical to those returned by @code{cfg_children()}, except that
the contents of the @code{cfg_path} and @code{cfg_pathlen} fields are
unspecified. 
         
@subheading NOTES: 

The @code{_POSIX_CFG} feature flag is defined to indicate
this service is available.

@page
@subsection cfg_children - Get Node Entries

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <cfg.h>

int cfg_children(
  CFG           *cfgp,
  int            options,
  CFGENT       **children
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_children()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item EACCES
Search permission is denied for any component of a pathname

@item EBADF
The @code{cfgp} argument does not refer to an open configuration space.

@item ELOOP
A loop exists in symbolic links encountered during resolution of 
a pathname.

@item ENAMETOOLONG
The length of a pathname exceeds @code{PATH_MAX}, or a pathname
component is longer than @code{NAME_MAX} while @code{_POSIX_NO_TRUNC} is
in effect.

@item EINVAL
The specified value of the @code{options} argument is invalid.

@item ENOENT
The named node does not exist.

@item ENOMEM
Not enough memory is available to create the necessary structures.

@end table

@subheading DESCRIPTION:

The first @code{cfg_children()} call after a @code{cfg_read()} returns 
information about the first node without children under the node
returned by @code{cfg_read()}.  Subsequent calls to @code{cfg_children()}
without the intervening @code{cfg_read()} shall return information
about the remaining nodes without children under that same node.

If @code{cfg_read()} has not yet been called for the configuration
traversal stream represented by @code{cfgp}, @code{cfg_children()}
returns a pointer to the first entry in a list of the nodes 
represented by the @code{pathnames} argument to @code{cfg_open()}.

In either case, the list is NULL-terminated, ordered by the 
user-specified comparison function, if any, and linked through the
@code{cfg_link} field.

@subheading NOTES:

The @code{_POSIX_CFG} feature flag is defined to indicate
this service is available.

@page
@subsection cfg_mark - Set Configuration Space Options

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <cfg.h>

int cfg_mark(
  CFG           *cfgp,
  CFGENT        *f,
  int            options
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_mark()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item EINVAL
The specified combination of the @code{cfgp} and @code{f} arguments is not 
supported by the implementation.

@item EINVAL
The specified value of the @code{options} argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{cfg_mark()} function modifies the subsequent behavior of
the @code{cfg} functions with regard to the node referenced by the structure
pointed to by the argument @code{f} or the configuration space referenced 
by the structure pointed to by the argument @code{cfgp}.

Exactly one of the @code{f} argument and the @code{cfgp} argument is NULL.

The value of the @code{options} argument is exactly one of the flags
specified in the following list: 

@table @b

@item CFG_AGAIN
If the @code{cfgp} argument is non-NULL, or the @code{f}
argument is NULL, or the structure referenced by @code{f}
is not the one most recently returned by @code{cfg_read()},
@code{cfg_mark()} returns an error.  Otherwise, the next 
call to the @code{cfg_read()} function returns the structure
referenced by @code{f} with the @code{cfg_info} field
reinitialized.  Subsequent behavior of the @code{cfg}
functions are based on the reinitialized value of 
@code{cfg_info}.

@item CFG_SKIP
If the @code{cfgp} argument is non-NULL, or the @code{f}
argument is NULL, or the structure referenced by @code{f}
is not one of those specified as accessible, or the structure
referenced by @code{f} is not for a node of type pre-order 
node, @code{cfg_mark()} returns an error.  Otherwise, no 
more structures for the node referenced by @code{f} or its 
descendants are returned by the @code{cfg_read()} function.

@item CFG_FOLLOW
If the @code{cfgp} argument is non-NULL, or the @code{f} 
argument is NULL, or the structure referenced by @code{f}
is not one of those specified as accessible, or the structure
referenced by @code{f} is not for a node of type symbolic link,
@code{cfg_mark()} returns an error.  Otherwise, the next
call to the @code{cfg_read()} function returns the structure
referenced by @code{f} with the @code{cfg_info} field reset
to reflect the target of the symbolic link instead of the 
symbolic link itself.  If the target of the link is node with
children, the pre-order return, followed by the return of 
structures referencing all of its descendants, followed by a 
post-order return, shall be done.

@end table

If the target of the symbolic link does not exist, the fields
of the structure by @code{cfg_read()} shall be unmodified, except
that the @code{cfg_info} field shall be reset to @code{CFG_SLNONE}.

@subheading NOTES:

The @code{_POSIX_CFG} feature flag is defined to indicate
this service is available.

@page
@subsection cfg_close - Close a Configuration Space

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <cfg.h>

int cfg_close(
  CFG           *cfgp
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_close()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item EBADF
The @code{cfgp} argument does not refer to an open configuration space
traversal stream.

@end table

@subheading DESCRIPTION:

The @code{cfg_close()} function closes a configuration space transversal
stream represented by the CFG structure pointed at by the @code{cfgp}
argument.  All system resources allocated for this configuration space
traversal stream should be deallocated.  Upon return, the value of 
@code{cfgp} need not point to an accessible object of type CFG.

@subheading NOTES:

The @code{_POSIX_CFG} feature flag is defined to indicate
this service is available.

@page
@subsection cfg_readdir - Reads a directory

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <dirent.h>

struct dirent *cfg_readdir(
  DIR   *dirp
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
Invalid file descriptor

@end table

@subheading DESCRIPTION:

The @code{cfg_readdir()} function returns a pointer to a structure @code{dirent}
representing the next directory entry from the directory stream pointed to
by @code{dirp}.  On end-of-file, NULL is returned.

The @code{cfg_readdir()} function may (or may not) return entries for . or .. Your
program should tolerate reading dot and dot-dot but not require them.

The data pointed to be @code{cfg_readdir()} may be overwritten by another call to
@code{readdir()} for the same directory stream.  It will not be overwritten by 
a call for another directory.

@subheading NOTES:

If @code{ptr} is not a pointer returned by @code{malloc()}, @code{calloc()}, or 
@code{realloc()} or has been deallocated with @code{free()} or @code{realloc()}, 
the results are not portable and are probably disastrous.

This function is not defined in the POSIX specification.  It is an extension 
provided by this implementation.

@page
@subsection cfg_umask - Sets a file creation mask.

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>

mode_t cfg_umask(
  mode_t cmask
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@subheading DESCRIPTION:

The @code{cfg_umask()} function sets the process node creation mask to @code{cmask}.
The file creation mask is used during @code{open()}, @code{creat()}, @code{mkdir()},
@code{mkfifo()} calls to turn off permission bits in the @code{mode} argument.
Bit positions that are set in @code{cmask} are cleared in the mode of the
created file.

The file creation mask is inherited across @code{fork()} and @code{exec()} calls.
This makes it possible to alter the default permission bits of created files.

@subheading NOTES: None

The @code{cmask} argument should have only permission bits set.  All other 
bits should be zero.

@page
@subsection cfg_chmod - Changes file mode.

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>

int cfg_chmod(
  const char *path,
  mode_t      mode
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_chmod()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item EACCES
Search permission is denied for a directory in a file's path prefix
@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is in
effect.
@item ENOENT
A file or directory does not exist.
@item ENOTDIR
A component of the specified pathname was not a directory when a directory
was expected.
@item EPERM
Operation is not permitted.  Process does not have the appropriate priviledges
or permissions to perform the requested operations.
@item EROFS
Read-only file system.

@end table

@subheading DESCRIPTION:

Set the file permission bits, the set user ID bit, and the set group ID bit 
for the file named by @code{path} to @code{mode}.  If the effective user ID 
does not match the owner of the node and the calling process does not have 
the appropriate privileges, @code{cfg_chmod()} returns -1 and sets @code{errno} to
@code{EPERM}.

@subheading NOTES:

@page
@subsection cfg_chown - Changes the owner and/or group of a file.

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <unistd.h>

int cfg_chown(
  const char *path,
  uid_t       owner,
  gid_t       group
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{cfg_chown()} returns a value of zero
and an unsuccessful call returns the @code{errno}.

@table @b
@item EACCES
Search permission is denied for a directory in a file's path prefix
@item EINVAL
Invalid argument
@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is in
effect.
@item ENOENT
A file or directory does not exist.
@item ENOTDIR
A component of the specified pathname was not a directory when a directory
was expected.
@item EPERM
Operation is not permitted.  Process does not have the appropriate priviledges
or permissions to perform the requested operations.
@item EROFS
Read-only file system.

@end table

@subheading DESCRIPTION:

The user ID and group ID of the file named by @code{path} are set to 
@code{owner} and @code{path}, respectively.

For regular files, the set group ID (S_ISGID) and set user ID (S_ISUID)
bits are cleared.

Some systems consider it a security violation to allow the owner of a file to
be changed,  If users are billed for disk space usage, loaning a file to 
another user could result in incorrect billing.  The @code{cfg_chown()} function
may be restricted to privileged users for some or all files.  The group ID can
still be changed to one of the supplementary group IDs.

@subheading NOTES:

This function may be restricted for some file.  The @code{pathconf} function
can be used to test the _PC_CHOWN_RESTRICTED flag.


