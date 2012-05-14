@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Object Services

@cindex object manipulation 

@section Introduction

RTEMS provides a collection of services to assist in the
management and usage of the objects created and utilized
via other managers.  These services assist in the 
manipulation of RTEMS objects independent of the API used
to create them.  The object related services provided by
RTEMS are:

@itemize @bullet
@c build_id
@item @code{@value{DIRPREFIX}build_name} - build object name from characters
@item @code{@value{DIRPREFIX}object_get_classic_name} - lookup name from Id
@item @code{@value{DIRPREFIX}object_get_name} - obtain object name as string
@item @code{@value{DIRPREFIX}object_set_name} - set object name
@item @code{@value{DIRPREFIX}object_id_get_api} - obtain API from Id
@item @code{@value{DIRPREFIX}object_id_get_class} - obtain class from Id
@item @code{@value{DIRPREFIX}object_id_get_node} - obtain node from Id
@item @code{@value{DIRPREFIX}object_id_get_index} - obtain index from Id
@item @code{@value{DIRPREFIX}build_id} - build object id from components
@item @code{@value{DIRPREFIX}object_id_api_minimum} - obtain minimum API value
@item @code{@value{DIRPREFIX}object_id_api_maximum} - obtain maximum API value
@item @code{@value{DIRPREFIX}object_id_api_minimum_class} - obtain minimum class value
@item @code{@value{DIRPREFIX}object_id_api_maximum_class} - obtain maximum class value
@item @code{@value{DIRPREFIX}object_get_api_name} - obtain API name
@item @code{@value{DIRPREFIX}object_get_api_class_name} - obtain class name
@item @code{@value{DIRPREFIX}object_get_class_information} - obtain class information
@end itemize

@section Background

@subsection APIs

RTEMS implements multiple APIs including an Internal API,
the Classic API, and the POSIX API.  These
APIs share the common foundation of SuperCore objects and
thus share object management code. This includes a common
scheme for object Ids and for managing object names whether
those names be in the thirty-two bit form used by the Classic
API or C strings.

The object Id contains a field indicating the API that
an object instance is associated with.  This field 
holds a numerically small non-zero integer.

@subsection Object Classes

Each API consists of a collection of managers.  Each manager
is responsible for instances of a particular object class.
Classic API Tasks and POSIX Mutexes example classes.  

The object Id contains a field indicating the class that
an object instance is associated with.  This field 
holds a numerically small non-zero integer.  In all APIs,
a class value of one is reserved for tasks or threads.

@subsection Object Names

Every RTEMS object which has an Id may also have a 
name associated with it.  Depending on the API, names
may be either thirty-two bit integers as in the Classic
API or strings as in the POSIX API.  

Some objects have Ids but do not have a defined way to associate
a name with them.  For example, POSIX threads have
Ids but per POSIX do not have names. In RTEMS, objects
not defined to have thirty-two bit names may have string
names assigned to them via the @code{@value{DIRPREFIX}object_set_name}
service.  The original impetus in providing this service
was so the normally anonymous POSIX threads could have 
a user defined name in CPU Usage Reports.

@section Operations

@subsection Decomposing and Recomposing an Object Id

Services are provided to decompose an object Id into its
subordinate components. The following services are used
to do this:

@itemize @bullet
@item @code{@value{DIRPREFIX}object_id_get_api}
@item @code{@value{DIRPREFIX}object_id_get_class}
@item @code{@value{DIRPREFIX}object_id_get_node}
@item @code{@value{DIRPREFIX}object_id_get_index}
@end itemize

The following C language example illustrates the
decomposition of an Id and printing the values.

@example
void printObjectId(rtems_id id)
@{
  printf(
    "API=%d Class=%d Node=%d Index=%d\n",
    rtems_object_id_get_api(id),
    rtems_object_id_get_class(id),
    rtems_object_id_get_node(id),
    rtems_object_id_get_index(id)
  ); 
@}
@end example

This prints the components of the Ids as integers.

It is also possible to construct an arbitrary Id using
the @code{@value{DIRPREFIX}build_id} service.  The following
C language example illustrates how to construct the
"next Id."

@example
rtems_id nextObjectId(rtems_id id)
@{
  return rtems_build_id(
    rtems_object_id_get_api(id),
    rtems_object_id_get_class(id),
    rtems_object_id_get_node(id),
    rtems_object_id_get_index(id) + 1
  ); 
@}
@end example

Note that this Id may not be valid in this
system or associated with an allocated object.

@subsection Printing an Object Id

RTEMS also provides services to associate the API and Class
portions of an Object Id with strings.  This allows the
application developer to provide more information about
an object in diagnostic messages.

In the following C language example, an Id is decomposed into
its constituent parts and "pretty-printed."

@example
void prettyPrintObjectId(rtems_id id)
@{
  int tmpAPI, tmpClass;

  tmpAPI   = rtems_object_id_get_api(id),
  tmpClass = rtems_object_id_get_class(id),

  printf(
    "API=%s Class=%s Node=%d Index=%d\n",
    rtems_object_get_api_name(tmpAPI),
    rtems_object_get_api_class_name(tmpAPI, tmpClass),
    rtems_object_id_get_node(id),
    rtems_object_id_get_index(id)
  ); 
@}
@end example

@section Directives

@c
@c
@c
@page
@subsection BUILD_NAME - Build object name from characters

@cindex build object name

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_build_name
@example
rtems_name rtems_build_name(
  uint8_t c1,
  uint8_t c2,
  uint8_t c3,
  uint8_t c4
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Build_Name(
   c1   : in     RTEMS.Unsigned8;
   c2   : in     RTEMS.Unsigned8;
   c3   : in     RTEMS.Unsigned8;
   c4   : in     RTEMS.Unsigned8;
   Name :    out RTEMS.Name
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

Returns a name constructed from the four characters.

@subheading DESCRIPTION:

This service takes the four characters provided as arguments
and constructs a thirty-two bit object name with @code{c1}
in the most significant byte and @code{c4} in the least
significant byte.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

@c
@c
@c
@page
@subsection OBJECT_GET_CLASSIC_NAME - Lookup name from id

@cindex get name from id
@cindex obtain name from id

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_build_name
@example
rtems_status_code rtems_object_get_classic_name(
  rtems_id      id,
  rtems_name   *name
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Object_Get_Classic_Name(
   ID     : in     RTEMS.ID;
   Name   :    out RTEMS.Name;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

@code{@value{RPREFIX}SUCCESSFUL} - name looked up successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - invalid name pointer@*
@code{@value{RPREFIX}INVALID_ID} - invalid object id@*

@subheading DESCRIPTION:

This service looks up the name for the object @code{id} specified
and, if found, places the result in @code{*name}.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

@c
@c
@c
@page
@subsection OBJECT_GET_NAME - Obtain object name as string

@cindex get object name as string
@cindex obtain object name as string

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_get_name
@example
char *rtems_object_get_name(
  rtems_id       id,
  size_t         length,
  char          *name
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Object_Get_Name(
   ID     : in     RTEMS.ID;
   Name   :    out RTEMS.Name;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

Returns a pointer to the name if successful or @code{NULL}
otherwise.

@subheading DESCRIPTION:

This service looks up the name of the object specified by
@code{id} and places it in the memory pointed to by @code{name}.
Every attempt is made to return name as a printable string even
if the object has the Classic API thirty-two bit style name.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

@c
@c
@c
@page
@subsection OBJECT_SET_NAME - Set object name

@cindex set object name

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_set_name
@example
rtems_status_code rtems_object_set_name(
  rtems_id       id,
  const char    *name
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Object_Set_Name(
   ID     : in     RTEMS.ID;
   Name   : in     String;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

@code{@value{RPREFIX}SUCCESSFUL} - name looked up successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - invalid name pointer@*
@code{@value{RPREFIX}INVALID_ID} - invalid object id@*

@subheading DESCRIPTION:

This service sets the name of @code{id} to that specified
by the string located at @code{name}.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

If the object specified by @code{id} is of a class that
has a string name, this method will free the existing
name to the RTEMS Workspace and allocate enough memory
from the RTEMS Workspace to make a copy of the string
located at @code{name}.

If the object specified by @code{id} is of a class that
has a thirty-two bit integer style name, then the first
four characters in @code{*name} will be used to construct
the name.
name to the RTEMS Workspace and allocate enough memory
from the RTEMS Workspace to make a copy of the string


@c
@c
@c
@page
@subsection OBJECT_ID_GET_API - Obtain API from Id

@cindex obtain API from id

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_id_get_api
@example
int rtems_object_id_get_api(
  rtems_id id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Object_Id_Get_API(
   ID  : in     RTEMS.ID;
   API :    out RTEMS.Unsigned32
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

Returns the API portion of the object Id.

@subheading DESCRIPTION:

This directive returns the API portion of the provided object @code{id}.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

This directive does NOT validate the @code{id} provided.

@c
@c
@c
@page
@subsection OBJECT_ID_GET_CLASS - Obtain Class from Id

@cindex obtain class from object id

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_id_get_class
@example
int rtems_object_id_get_class(
  rtems_id id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Object_Id_Get_Class(
   ID        : in     RTEMS.ID;
   The_Class :    out RTEMS.Unsigned32
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

Returns the class portion of the object Id.

@subheading DESCRIPTION:

This directive returns the class portion of the provided object @code{id}.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

This directive does NOT validate the @code{id} provided.

@c
@c
@c
@page
@subsection OBJECT_ID_GET_NODE - Obtain Node from Id

@cindex obtain node from object id

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_id_get_node
@example
int rtems_object_id_get_node(
  rtems_id id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Object_Id_Get_Node(
   ID   : in     RTEMS.ID;
   Node :    out RTEMS.Unsigned32
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

Returns the node portion of the object Id.

@subheading DESCRIPTION:

This directive returns the node portion of the provided object @code{id}.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

This directive does NOT validate the @code{id} provided.

@c
@c
@c
@page
@subsection OBJECT_ID_GET_INDEX - Obtain Index from Id

@cindex obtain index from object id

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_id_get_index
@example
int rtems_object_id_get_index(
  rtems_id id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Object_Id_Get_Index(
   ID    : in     RTEMS.ID;
   Index :    out RTEMS.Unsigned32
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

Returns the index portion of the object Id.

@subheading DESCRIPTION:

This directive returns the index portion of the provided object @code{id}.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

This directive does NOT validate the @code{id} provided.

@c
@c
@c
@page
@subsection BUILD_ID - Build Object Id From Components

@cindex build object id from components

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_build_id
@example
rtems_id rtems_build_id(
  int the_api,
  int the_class,
  int the_node,
  int the_index
);
@end example
@end ifset

@ifset is-Ada
@example
function Build_Id(
   the_api   : in     RTEMS.Unsigned32;
   the_class : in     RTEMS.Unsigned32;
   the_node  : in     RTEMS.Unsigned32;
   the_index : in     RTEMS.Unsigned32
) return RTEMS.Id;
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

Returns an object Id constructed from the provided arguments.

@subheading DESCRIPTION:

This service constructs an object Id from the provided
@code{the_api}, @code{the_class}, @code{the_node}, and @code{the_index}.


@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

This directive does NOT validate the arguments provided
or the Object id returned.

@c
@c
@c
@page
@subsection OBJECT_ID_API_MINIMUM - Obtain Minimum API Value

@cindex obtain minimum API value

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_id_api_minimum
@example
int rtems_object_id_api_minimum(void);
@end example
@end ifset

@ifset is-Ada
@example
function Object_Id_API_Minimum return RTEMS.Unsigned32;
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

Returns the minimum valid for the API portion of an object Id.

@subheading DESCRIPTION:

This service returns the minimum valid for the API portion of
an object Id.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

@c
@c
@c
@page
@subsection OBJECT_ID_API_MAXIMUM - Obtain Maximum API Value

@cindex obtain maximum API value

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_id_api_maximum
@example
int rtems_object_id_api_maximum(void);
@end example
@end ifset

@ifset is-Ada
@example
function Object_Id_API_Maximum return RTEMS.Unsigned32;
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

Returns the maximum valid for the API portion of an object Id.

@subheading DESCRIPTION:

This service returns the maximum valid for the API portion of
an object Id.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

@c
@c
@c
@page
@subsection OBJECT_API_MINIMUM_CLASS - Obtain Minimum Class Value

@cindex obtain minimum class value

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_api_minimum_class
@example
int rtems_object_api_minimum_class(
  int api
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Object_API_Minimum_Class(
   API     : in     RTEMS.Unsigned32;
   Minimum :    out RTEMS.Unsigned32
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

If @code{api} is not valid, -1 is returned.

If successful, this service returns the minimum valid for the class
portion of an object Id for the specified @code{api}.

@subheading DESCRIPTION:

This service returns the minimum valid for the class portion of
an object Id for the specified @code{api}.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

@c
@c
@c
@page
@subsection OBJECT_API_MAXIMUM_CLASS - Obtain Maximum Class Value

@cindex obtain maximum class value

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_api_maximum_class
@example
int rtems_object_api_maximum_class(
  int api
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Object_API_Maximum_Class(
   API     : in     RTEMS.Unsigned32;
   Maximum :    out RTEMS.Unsigned32
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

If @code{api} is not valid, -1 is returned.

If successful, this service returns the maximum valid for the class
portion of an object Id for the specified @code{api}.

@subheading DESCRIPTION:

This service returns the maximum valid for the class portion of
an object Id for the specified @code{api}.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.


@c
@c
@c
@page
@subsection OBJECT_GET_API_NAME - Obtain API Name

@cindex obtain API name

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_get_api_name
@example
const char *rtems_object_get_api_name(
  int api
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Object_Get_API_Name(
   API  : in     RTEMS.Unsigned32;
   Name :    out String
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

If @code{api} is not valid, the string @code{"BAD API"} is returned.

If successful, this service returns a pointer to a string 
containing the name of the specified @code{api}.

@subheading DESCRIPTION:

This service returns the name of the specified @code{api}. 

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

The string returned is from constant space.  Do not modify
or free it.

@c
@c
@c
@page
@subsection OBJECT_GET_API_CLASS_NAME - Obtain Class Name

@cindex obtain class name

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_get_api_class_name
@example
const char *rtems_object_get_api_class_name(
  int the_api,
  int the_class
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Object_Get_API_Class_Name(
   The_API   : in     RTEMS.Unsigned32;
   The_Class : in     RTEMS.Unsigned32;
   Name      :    out String
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

If @code{the_api} is not valid, the string @code{"BAD API"} is returned.

If @code{the_class} is not valid, the string @code{"BAD CLASS"} is returned.

If successful, this service returns a pointer to a string 
containing the name of the specified @code{the_api}/@code{the_class} pair.

@subheading DESCRIPTION:

This service returns the name of the object class indicated by the
specified @code{the_api} and @code{the_class}.

@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

The string returned is from constant space.  Do not modify
or free it.

@c
@c
@c
@page
@subsection OBJECT_GET_CLASS_INFORMATION - Obtain Class Information

@cindex obtain class information

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_object_get_class_information
@example
rtems_status_code rtems_object_get_class_information(
  int                                 the_api,
  int                                 the_class,
  rtems_object_api_class_information *info
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Object_Get_Class_Information(
   The_API   : in     RTEMS.Unsigned32;
   The_Class : in     RTEMS.Unsigned32;
   Info      :    out RTEMS.Object_API_Class_Information;
   Result    :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES
@code{@value{RPREFIX}SUCCESSFUL} - information obtained successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{info} is NULL@*
@code{@value{RPREFIX}INVALID_NUMBER} - invalid @code{api} or @code{the_class}

If successful, the structure located at @code{info} will be filled
in with information about the specified @code{api}/@code{the_class} pairing.

@subheading DESCRIPTION:

This service returns information about the object class indicated by the
specified @code{api} and @code{the_class}. This structure is defined as
follows:

@ifset is-C
@example
typedef struct @{
  rtems_id  minimum_id;
  rtems_id  maximum_id;
  int       maximum;
  bool      auto_extend;
  int       unallocated;
@} rtems_object_api_class_information;
@end example
@end ifset

@ifset is-Ada
@example

@end example
@end ifset


@subheading NOTES:

This directive is strictly local and does not impact task scheduling.

