@c
@c Copyright 2015 embedded brains GmbH
@c All rights reserved.

@chapter Linker Sets

@cindex linkersets

@section Introduction

Linker sets are a flexible means to create arrays of items out of a set of
object files at link-time.  For example its possible to define an item @emph{I}
of type @emph{T} in object file @emph{A} and an item @emph{J} of type @emph{T}
in object file @emph{B} to be a member of a linker set @emph{S}.  The linker
will then collect these two items @emph{I} and @emph{J} and place them in
consecutive memory locations, so that they can be accessed like a normal array
defined in one object file.  The size of a linker set is defined by its begin
and end markers.  A linker set may be empty.  It should only contain items of
the same type.

The following macros are provided to create, populate and use linker sets.

@itemize @bullet
@item @code{RTEMS_LINKER_SET_BEGIN} - Designator of the linker set begin marker
@item @code{RTEMS_LINKER_SET_END} - Designator of the linker set end marker
@item @code{RTEMS_LINKER_SET_SIZE} - The linker set size in characters
@item @code{RTEMS_LINKER_ROSET_DECLARE} - Declares a read-only linker set
@item @code{RTEMS_LINKER_ROSET} - Defines a read-only linker set
@item @code{RTEMS_LINKER_ROSET_ITEM_DECLARE} - Declares a read-only linker set item
@item @code{RTEMS_LINKER_ROSET_ITEM_REFERENCE} - References a read-only linker set item
@item @code{RTEMS_LINKER_ROSET_ITEM} - Defines a read-only linker set item
@item @code{RTEMS_LINKER_ROSET_ITEM_ORDERED} - Defines an ordered read-only linker set item
@item @code{RTEMS_LINKER_RWSET_DECLARE} - Declares a read-write linker set
@item @code{RTEMS_LINKER_RWSET} - Defines a read-write linker set
@item @code{RTEMS_LINKER_RWSET_ITEM_DECLARE} - Declares a read-write linker set item
@item @code{RTEMS_LINKER_RWSET_ITEM_REFERENCE} - References a read-write linker set item
@item @code{RTEMS_LINKER_RWSET_ITEM} - Defines a read-write linker set item
@item @code{RTEMS_LINKER_RWSET_ITEM_ORDERED} - Defines an ordered read-write linker set item
@end itemize

@section Background

Linker sets are used not only in RTEMS, but also for example in Linux, in
FreeBSD, for the GNU C constructor extension and for global C++ constructors.
They provide a space efficient and flexible means to initialize modules.  A
linker set consists of
@itemize @bullet
@item dedicated input sections for the linker (e.g. @code{.ctors} and
@code{.ctors.*} in the case of global constructors),
@item a begin marker (e.g. provided by @code{crtbegin.o}, and
@item an end marker (e.g. provided by @code{ctrend.o}).
@end itemize
A module may place a certain data item into the dedicated input section.  The
linker will collect all such data items in this section and creates a begin and
end marker.  The initialization code can then use the begin and end markers to
find all the collected data items (e.g. pointers to initialization functions).

In the linker command file of the GNU linker we need the following output
section descriptions.
@example
@group
/* To be placed in a read-only memory region */
.rtemsroset : @{
  KEEP (*(SORT(.rtemsroset.*)))
@}

/* To be placed in a read-write memory region */
.rtemsrwset : @{
  KEEP (*(SORT(.rtemsrwset.*)))
@}
@end group
@end example
The @code{KEEP()} ensures that a garbage collection by the linker will not
discard the content of this section.  This would normally be the case since the
linker set items are not referenced directly.  The @code{SORT()} directive
sorts the input sections lexicographically.  Please note the lexicographical
order of the @code{.begin}, @code{.content} and @code{.end} section name parts
in the RTEMS linker sets macros which ensures that the position of the begin
and end markers are right.

So, what is the benefit of using linker sets to initialize modules?  It can be
used to initialize and include only those RTEMS managers and other components
which are used by the application.  For example, in case an application uses
message queues, it must call @code{rtems_message_queue_create()}.  In the
module implementing this function, we can place a linker set item and register
the message queue handler constructor.  Otherwise, in case the application does
not use message queues, there will be no reference to the
@code{rtems_message_queue_create()} function and the constructor is not
registered, thus nothing of the message queue handler will be in the final
executable.

For an example see test program @file{sptests/splinkersets01}.

@section Directives

@macro linkersetsset
The @code{set} parameter itself must be a valid C designator on which no macro
expansion is performed.  It uniquely identifies the linker set.
@end macro

@macro linkersetstype
The @code{type} parameter defines the type of the linker set items.  The type
must be the same for all macro invocations of a particular linker set.
@end macro

@macro linkersetsitem
The @code{item} parameter itself must be a valid C designator on which no macro
expansion is performed.  It uniquely identifies an item in the linker set.
@end macro

@macro linkersetsorder
The @code{order} parameter must be a valid linker input section name part on
which macro expansion is performed.  The items are lexicographically ordered
according to the @code{order} parameter within a linker set.  Ordered items are
placed before unordered items in the linker set.
@end macro

@page

@subsection RTEMS_LINKER_SET_BEGIN - Designator of the linker set begin marker

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_SET_BEGIN
@example
volatile type *begin = RTEMS_LINKER_SET_BEGIN( set );
@end example

@subheading DESCRIPTION:

This macro generates the designator of the begin marker of the linker set
identified by @code{set}.  The item at the begin marker address is the first
member of the linker set if it exists, e.g. the linker set is not empty.  A
linker set is empty, if and only if the begin and end markers have the same
address.
@linkersetsset

@page

@subsection RTEMS_LINKER_SET_END - Designator of the linker set end marker

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_SET_END
@example
volatile type *end = RTEMS_LINKER_SET_END( set );
@end example

@subheading DESCRIPTION:

This macro generates the designator of the end marker of the linker set
identified by @code{set}.  The item at the end marker address is not a member
of the linker set.  @linkersetsset

@page

@subsection RTEMS_LINKER_SET_SIZE - The linker set size in characters

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_SET_SIZE
@example
size_t size = RTEMS_LINKER_SET_SIZE( set );
@end example

@subheading DESCRIPTION:

This macro returns the size of the linker set identified by @code{set} in
characters.  @linkersetsset

@page

@subsection RTEMS_LINKER_ROSET_DECLARE - Declares a read-only linker set

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_ROSET_DECLARE
@example
RTEMS_LINKER_ROSET_DECLARE( set, type );
@end example

@subheading DESCRIPTION:

This macro generates declarations for the begin and end markers of a read-only
linker set identified by @code{set}.  @linkersetsset @linkersetstype

@page

@subsection RTEMS_LINKER_ROSET - Defines a read-only linker set

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_ROSET
@example
RTEMS_LINKER_ROSET( set, type );
@end example

@subheading DESCRIPTION:

This macro generates definitions for the begin and end markers of a read-only
linker set identified by @code{set}.  @linkersetsset @linkersetstype

@page

@subsection RTEMS_LINKER_ROSET_ITEM_DECLARE - Declares a read-only linker set item

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_ROSET_ITEM_DECLARE
@example
RTEMS_LINKER_ROSET_ITEM_DECLARE( set, type, item );
@end example

@subheading DESCRIPTION:

This macro generates a declaration of an item contained in the read-only linker
set identified by @code{set}.  @linkersetsset @linkersetstype @linkersetsitem

@page

@subsection RTEMS_LINKER_ROSET_ITEM_REFERENCE - References a read-only linker set item

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_ROSET_ITEM_REFERENCE
@example
RTEMS_LINKER_ROSET_ITEM_REFERENCE( set, type, item );
@end example

@subheading DESCRIPTION:

This macro generates a reference to an item contained in the read-only linker set
identified by @code{set}.  @linkersetsset @linkersetstype @linkersetsitem

@page

@subsection RTEMS_LINKER_ROSET_ITEM - Defines a read-only linker set item

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_ROSET_ITEM
@example
RTEMS_LINKER_ROSET_ITEM( set, type, item );
@end example

@subheading DESCRIPTION:

This macro generates a definition of an item contained in the read-only linker set
identified by @code{set}.  @linkersetsset @linkersetstype @linkersetsitem

@page

@subsection RTEMS_LINKER_ROSET_ITEM_ORDERED - Defines an ordered read-only linker set item

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_ROSET_ITEM_ORDERED
@example
RTEMS_LINKER_ROSET_ITEM_ORDERED( set, type, item, order );
@end example

@subheading DESCRIPTION:

This macro generates a definition of an ordered item contained in the read-only
linker set identified by @code{set}.  @linkersetsset @linkersetstype
@linkersetsitem @linkersetsorder

@subheading NOTES:

To be resilient to typos in the order parameter, it is recommended to use the
following construct in macros defining items for a particular linker set (see
enum in @code{XYZ_ITEM()}).

@example
#include <rtems/linkersets.h>

typedef struct @{
  int foo;
@} xyz_item;

/* The XYZ-order defines */
#define XYZ_ORDER_FIRST 0x00001000
#define XYZ_ORDER_AND_SO_ON 0x00002000

/* Defines an ordered XYZ-item */
#define XYZ_ITEM( item, order ) \
  enum @{ xyz_##item = order - order @}; \
  RTEMS_LINKER_ROSET_ITEM_ORDERED( \
    xyz, const xyz_item *, item, order \
  ) = @{ &item @}

/* Example item */
static const xyz_item some_item = @{ 123 @};
XYZ_ITEM( some_item, XYZ_ORDER_FIRST );
@end example

@page

@subsection RTEMS_LINKER_RWSET_DECLARE - Declares a read-write linker set

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_RWSET_DECLARE
@example
RTEMS_LINKER_RWSET_DECLARE( set, type );
@end example

@subheading DESCRIPTION:

This macro generates declarations for the begin and end markers of a read-write
linker set identified by @code{set}.  @linkersetsset @linkersetstype

@page

@subsection RTEMS_LINKER_RWSET - Defines a read-write linker set

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_RWSET
@example
RTEMS_LINKER_RWSET( set, type );
@end example

@subheading DESCRIPTION:

This macro generates definitions for the begin and end markers of a read-write
linker set identified by @code{set}.  @linkersetsset @linkersetstype

@page

@subsection RTEMS_LINKER_RWSET_ITEM_DECLARE - Declares a read-write linker set item

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_RWSET_ITEM_DECLARE
@example
RTEMS_LINKER_RWSET_ITEM_DECLARE( set, type, item );
@end example

@subheading DESCRIPTION:

This macro generates a declaration of an item contained in the read-write linker
set identified by @code{set}.  @linkersetsset @linkersetstype @linkersetsitem

@page

@subsection RTEMS_LINKER_RWSET_ITEM_REFERENCE - References a read-write linker set item

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_RWSET_ITEM_REFERENCE
@example
RTEMS_LINKER_RWSET_ITEM_REFERENCE( set, type, item );
@end example

@subheading DESCRIPTION:

This macro generates a reference to an item contained in the read-write linker set
identified by @code{set}.  @linkersetsset @linkersetstype @linkersetsitem

@page

@subsection RTEMS_LINKER_RWSET_ITEM - Defines a read-write linker set item

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_RWSET_ITEM
@example
RTEMS_LINKER_RWSET_ITEM( set, type, item );
@end example

@subheading DESCRIPTION:

This macro generates a definition of an item contained in the read-write linker set
identified by @code{set}.  @linkersetsset @linkersetstype @linkersetsitem

@page

@subsection RTEMS_LINKER_RWSET_ITEM_ORDERED - Defines an ordered read-write linker set item

@subheading CALLING SEQUENCE:

@findex RTEMS_LINKER_RWSET_ITEM_ORDERED
@example
RTEMS_LINKER_RWSET_ITEM_ORDERED( set, type, item, order );
@end example

@subheading DESCRIPTION:

This macro generates a definition of an ordered item contained in the read-write
linker set identified by @code{set}.  @linkersetsset @linkersetstype
@linkersetsitem @linkersetsorder

@subheading NOTES:

To be resilient to typos in the order parameter, it is recommended to use the
following construct in macros defining items for a particular linker set (see
enum in @code{XYZ_ITEM()}).

@example
#include <rtems/linkersets.h>

typedef struct @{
  int foo;
@} xyz_item;

/* The XYZ-order defines */
#define XYZ_ORDER_FIRST 0x00001000
#define XYZ_ORDER_AND_SO_ON 0x00002000

/* Defines an ordered XYZ-item */
#define XYZ_ITEM( item, order ) \
  enum @{ xyz_##item = order - order @}; \
  RTEMS_LINKER_RWSET_ITEM_ORDERED( \
    xyz, const xyz_item *, item, order \
  ) = @{ &item @}

/* Example item */
static const xyz_item some_item = @{ 123 @};
XYZ_ITEM( some_item, XYZ_ORDER_FIRST );
@end example
