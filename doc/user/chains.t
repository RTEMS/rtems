@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Chains

@cindex chains

@section Introduction

The Chains API is an interface to the Super Core (score) chain
implementation. The Super Core uses chains for all list type
functions. This includes wait queues and task queues. The Chains API
provided by RTEMS is:

@itemize @bullet
@c build_id
@item @code{@value{DIRPREFIX}chain_node} - Chain node used in user objects
@item @code{@value{DIRPREFIX}chain_control} - Chain control node
@item @code{@value{DIRPREFIX}chain_initialize} - initialize the chain with nodes
@item @code{@value{DIRPREFIX}chain_initialize_empty} - initialize the chain as empty
@item @code{@value{DIRPREFIX}chain_is_null_node} - Is the node NULL ?
@item @code{@value{DIRPREFIX}chain_head} - Return the chain's head
@item @code{@value{DIRPREFIX}chain_tail} - Return the chain's tail
@item @code{@value{DIRPREFIX}chain_are_nodes_equal} - Are the node's equal ?
@item @code{@value{DIRPREFIX}chain_is_empty} - Is the chain empty ?
@item @code{@value{DIRPREFIX}chain_is_first} - Is the Node the first in the chain ?
@item @code{@value{DIRPREFIX}chain_is_last} - Is the Node the last in the chain ? 
@item @code{@value{DIRPREFIX}chain_has_only_one_node} - Does the node have one node ?
@item @code{@value{DIRPREFIX}chain_is_head} - Is the node the head ?
@item @code{@value{DIRPREFIX}chain_is_tail} - Is the node the tail ?
@item @code{@value{DIRPREFIX}chain_extract} - Extract the node from the chain
@item @code{@value{DIRPREFIX}chain_extract_unprotected} - Extract the node from the chain (unprotected)
@item @code{@value{DIRPREFIX}chain_get} - Return the first node on the chain
@item @code{@value{DIRPREFIX}chain_get_unprotected} - Return the first node on the chain (unprotected)
@item @code{@value{DIRPREFIX}chain_insert} - Insert the node into the chain
@item @code{@value{DIRPREFIX}chain_insert_unprotected} - Insert the node into the chain (unprotected)
@item @code{@value{DIRPREFIX}chain_append} - Append the node to chain
@item @code{@value{DIRPREFIX}chain_append_unprotected} - Append the node to chain (unprotected)
@item @code{@value{DIRPREFIX}chain_prepend} - Prepend the node to the end of the chain
@item @code{@value{DIRPREFIX}chain_prepend_unprotected} - Prepend the node to chain (unprotected)
@end itemize

@section Background

The Chains API maps to the Super Core Chains API. Chains are
implemented as a double linked list of nodes anchored to a control
node. The list starts at the control node and is terminated at the
control node. A node has previous and next pointers. Being a double
linked list nodes can be inserted and removed without the need to
travse the chain.

Chains have a small memory footprint and can be used in interrupt
service routines and are thread safe in a multi-threaded
environment. The directives list which operations disable interrupts.

Chains are very useful in Board Support packages and applications.

@subsection Nodes

A chain is made up from nodes that orginate from a chain control
object. A node is of type @code{@value{DIRPREFIX}chain_node}. The node
is designed to be part of a user data structure and a cast is used to
move from the node address to the user data structure address. For
example:

@example
typedef struct foo
@{
  @value{DIRPREFIX}chain_node node;
  int              bar;
@} foo;
@end example

creates a type @code{foo} that can be placed on a chain. To get the
foo structure from the list you perform the following:

@example
foo* get_foo(@value{DIRPREFIX}chain_control* control)
@{
  return (foo*) @value{DIRPREFIX}chain_get(control);
@}
@end example

The node is placed at the start of the user's structure to allow the
node address on the chain to be easly cast to the user's structure
address. 

@subsection Controls

A chain is anchored with a control object. Chain control provide the
user with access to the nodes on the chain. The control is head of the
node.


@example
       Control
        first ------------------------>
        permanent_null <--------------- NODE
        last ------------------------->
@end example

The implementation does not require special checks for manipulating
the first and last nodes on the chain. To accomplish this the
@code{@value{DIRPREFIX}chain_control} structure is treated as two
overlapping @code{@value{DIRPREFIX}chain_node} structures.  The
permanent head of the chain overlays a node structure on the first and
@code{permanent_null} fields.  The @code{permanent_tail} of the chain
overlays a node structure on the @code{permanent_null} and @code{last}
elements of the structure.

@section Operations

@subsection Multi-threading

Chains are designed to be used in a multi-threading environment. The
directives list which operations mask interrupts. Chains supports
tasks and interrupt service routines appending and extracting nodes
with out the need for extra locks. Chains how-ever cannot insure the
integrity of a chain for all operations. This is the responsibility of
the user. For example an interrupt service routine extracting nodes
while a task is iterating over the chain can have unpredictable
results.

@subsection Creating a Chain

To create a chain you need to declare a chain control then add nodes
to the control. Consider a user structure and chain control:

@example
typedef struct foo
@{
  @value{DIRPREFIX}chain_node node;
  uint8_t char*    data;
@} foo;

@value{DIRPREFIX}chain_control chain;
@end example

Add nodes with the following code:

@example
@value{DIRPREFIX}chain_initialize_empty (&chain);

for (i = 0; i < count; i++)
@{
  foo* bar = malloc (sizeof (foo));
  if (!bar)
    return -1;
  bar->data = malloc (size);
  @value{DIRPREFIX}chain_append (&chain, &bar->node);
@}
@end example

The chain is initialized and the nodes allocated and appended to the
chain. This is an example of a pool of buffers.

@subsection Iterating a Chain

@cindex chain iterate

Iterating a chain is a common function. The example shows how to
iterate the buffer pool chain created in the last section to find
buffers starting with a specific string. If the buffer is located it is
extracted from the chain and placed on another chain:

@example
void foobar (const char*          match,
             @value{DIRPREFIX}chain_control* chain,
             @value{DIRPREFIX}chain_control* out)
@{
  @value{DIRPREFIX}chain_node* node;
  foo*              bar;

  @value{DIRPREFIX}chain_initialize_empty (out);

  node = chain->first;

  while (!@value{DIRPREFIX}chain_is_tail (chain, node))
  @{
    bar = (foo*) node;
    rtems_chain_node* next_node = node->next;

    if (strcmp (match, bar->data) == 0)
    @{
      @value{DIRPREFIX}chain_extract (node);
      @value{DIRPREFIX}chain_append (out, node);
    @}

    node = next_node; 
  @}
@}
@end example

@section Directives

The section details the Chains directives.

@c
@c Initialize this Chain With Nodes
@c
@page
@subsection Initialize Chain With Nodes

@cindex chain initialize

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_initialize
@example
void @value{DIRPREFIX}chain_initialize(
  @value{DIRPREFIX}chain_control *the_chain,
  void                *starting_address,
  size_t               number_nodes,
  size_t               node_size
)
@end example
@end ifset

@subheading RETURNS

Returns nothing.

@subheading DESCRIPTION:

This function take in a pointer to a chain control and initializes it
to contain a set of chain nodes.  The chain will contain @code{number_nodes}
chain nodes from the memory pointed to by @code{start_address}.  Each node
is assumed to be @code{node_size} bytes.

@subheading NOTES:

This call will discard any nodes on the chain.

This call does NOT inititialize any user data on each node.

@c
@c Initialize this Chain as Empty
@c
@page
@subsection Initialize Empty

@cindex chain initialize empty

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_initialize_empty
@example
void @value{DIRPREFIX}chain_initialize_empty(
  @value{DIRPREFIX}chain_control *the_chain
);
@end example
@end ifset

@subheading RETURNS

Returns nothing.

@subheading DESCRIPTION:

This function take in a pointer to a chain control and initializes it
to empty.

@subheading NOTES:

This call will discard any nodes on the chain.

@c
@c
@c
@page
@subsection Is Null Node ?

@cindex chain is node null

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_is_null_node
@example
bool @value{DIRPREFIX}chain_is_null_node(
  const @value{DIRPREFIX}chain_node *the_node
);
@end example
@end ifset

@subheading RETURNS

Returns @code{true} is the node point is NULL and @code{false} if the node is not
NULL.

@subheading DESCRIPTION:

Tests the node to see if it is a NULL returning @code{true} if a null.

@c
@c
@c
@page
@subsection Head

@cindex chain get head

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_head
@example
@value{DIRPREFIX}chain_node *@value{DIRPREFIX}chain_head(
  @value{DIRPREFIX}chain_control *the_chain
)
@end example
@end ifset

@subheading RETURNS

Returns the permanent head node of the chain.

@subheading DESCRIPTION:

This function returns a pointer to the first node on the chain.

@c
@c
@c
@page
@subsection Tail

@cindex chain get tail

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_tail
@example
@value{DIRPREFIX}chain_node *@value{DIRPREFIX}chain_tail(
  @value{DIRPREFIX}chain_control *the_chain
);
@end example
@end ifset

@subheading RETURNS

Returns the permanent tail node of the chain.

@subheading DESCRIPTION:

This function returns a pointer to the last node on the chain.

@c
@c
@c
@page
@subsection Are Two Nodes Equal ?

@cindex chare are nodes equal

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_are_nodes_equal
@example
bool @value{DIRPREFIX}chain_are_nodes_equal(
  const @value{DIRPREFIX}chain_node *left,
  const @value{DIRPREFIX}chain_node *right
);
@end example
@end ifset

@subheading RETURNS

This function returns @code{true} if the left node and the right node are
equal, and @code{false} otherwise.

@subheading DESCRIPTION:

This function returns @code{true} if the left node and the right node are
equal, and @code{false} otherwise.

@c
@c
@c
@page
@subsection Is the Chain Empty

@cindex chain is chain empty

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_is_empty
@example
bool @value{DIRPREFIX}chain_is_empty(
  @value{DIRPREFIX}chain_control *the_chain
);
@end example
@end ifset

@subheading RETURNS

This function returns @code{true} if there a no nodes on the chain and @code{false}
otherwise.

@subheading DESCRIPTION:

This function returns @code{true} if there a no nodes on the chain and @code{false}
otherwise.

@c
@c
@c
@page
@subsection Is this the First Node on the Chain ?

@cindex chain is node the first

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_is_first
@example
bool @value{DIRPREFIX}chain_is_first(
  const @value{DIRPREFIX}chain_node *the_node
);
@end example
@end ifset

@subheading RETURNS

This function returns @code{true} if the node is the first node on a chain
and @code{false} otherwise.

@subheading DESCRIPTION:

This function returns @code{true} if the node is the first node on a chain
and @code{false} otherwise.

@c
@c
@c
@page
@subsection Is this the Last Node on the Chain ?

@cindex chain is node the last

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_is_last
@example
bool @value{DIRPREFIX}chain_is_last(
  const @value{DIRPREFIX}chain_node *the_node
);
@end example
@end ifset

@subheading RETURNS

This function returns @code{true} if the node is the last node on a chain and
@code{false} otherwise.

@subheading DESCRIPTION:

This function returns @code{true} if the node is the last node on a chain and
@code{false} otherwise.

@c
@c
@c
@page
@subsection Does this Chain have only One Node ?

@cindex chain only one node

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_has_only_one_node
@example
bool @value{DIRPREFIX}chain_has_only_one_node(
  const @value{DIRPREFIX}chain_control *the_chain
);
@end example
@end ifset

@subheading RETURNS

This function returns @code{true} if there is only one node on the chain and
@code{false} otherwise.

@subheading DESCRIPTION:

This function returns @code{true} if there is only one node on the chain and
@code{false} otherwise.

@c
@c
@c
@page
@subsection Is this Node the Chain Head ?

@cindex chain is node the head

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_is_head
@example
bool @value{DIRPREFIX}chain_is_head(
  @value{DIRPREFIX}chain_control    *the_chain,
  @value{DIRPREFIX}const chain_node *the_node
);
@end example
@end ifset

@subheading RETURNS

This function returns @code{true} if the node is the head of the chain and
@code{false} otherwise.

@subheading DESCRIPTION:

This function returns @code{true} if the node is the head of the chain and
@code{false} otherwise.

@c
@c
@c
@page
@subsection Is this Node the Chain Tail ?

@cindex chain is node the tail

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_is_tail
@example
bool @value{DIRPREFIX}chain_is_tail(
  @value{DIRPREFIX}chain_control    *the_chain,
  const @value{DIRPREFIX}chain_node *the_node
)
@end example
@end ifset

@subheading RETURNS

This function returns @code{true} if the node is the tail of the chain and
@code{false} otherwise.

@subheading DESCRIPTION:

This function returns @code{true} if the node is the tail of the chain and
@code{false} otherwise.

@c
@c
@c
@page
@subsection Extract a Node

@cindex chain extract a node

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_extract
@example
void @value{DIRPREFIX}chain_extract(
  @value{DIRPREFIX}chain_node *the_node
);
@end example
@end ifset

@subheading RETURNS

Returns nothing.

@subheading DESCRIPTION:

This routine extracts the node from the chain on which it resides.

@subheading NOTES:

Interrupts are disabled while extracting the node to ensure the
atomicity of the operation.

Use @code{@value{DIRPREFIX}chain_extract_unprotected()} to avoid disabling of
interrupts.

@c
@c
@c
@page
@subsection Get the First Node

@cindex chain get first node

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_get
@example
@value{DIRPREFIX}chain_node *@value{DIRPREFIX}chain_get(
  @value{DIRPREFIX}chain_control *the_chain
);
@end example
@end ifset

@subheading RETURNS

Returns a pointer a node. If a node was removed, then a pointer to
that node is returned. If the chain was empty, then NULL is
returned.

@subheading DESCRIPTION:

This function removes the first node from the chain and returns a
pointer to that node.  If the chain is empty, then NULL is returned.

@subheading NOTES:

Interrupts are disabled while obtaining the node to ensure the
atomicity of the operation.

Use @code{@value{DIRPREFIX}chain_get_unprotected()} to avoid disabling of
interrupts.

@c
@c
@c
@page
@subsection Insert a Node

@cindex chain insert a node

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_insert
@example
void @value{DIRPREFIX}chain_insert(
  @value{DIRPREFIX}chain_node *after_node,
  @value{DIRPREFIX}chain_node *the_node
);
@end example
@end ifset

@subheading RETURNS

Returns nothing.

@subheading DESCRIPTION:

This routine inserts a node on a chain immediately following the
specified node.

@subheading NOTES:

Interrupts are disabled during the insert to ensure the atomicity of
the operation.

Use @code{@value{DIRPREFIX}chain_insert_unprotected()} to avoid disabling of
interrupts.

@c
@c
@c
@page
@subsection Append a Node

@cindex chain append a node

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_append
@example
void @value{DIRPREFIX}chain_append(
  @value{DIRPREFIX}chain_control *the_chain,
  @value{DIRPREFIX}chain_node    *the_node
);
@end example
@end ifset

@subheading RETURNS

Returns nothing.

@subheading DESCRIPTION:

This routine appends a node to the end of a chain.

@subheading NOTES:

Interrupts are disabled during the append to ensure the atomicity of
the operation.

Use @code{@value{DIRPREFIX}chain_append_unprotected()} to avoid disabling of
interrupts.

@c
@c
@c
@page
@subsection Prepend a Node

@cindex prepend node

@subheading CALLING SEQUENCE:

@ifset is-C
@findex @value{DIRPREFIX}chain_prepend
@example
void @value{DIRPREFIX}chain_prepend(
  @value{DIRPREFIX}chain_control *the_chain,
  @value{DIRPREFIX}chain_node    *the_node
);
@end example
@end ifset

@subheading RETURNS

Returns nothing.

@subheading DESCRIPTION:

This routine prepends a node to the front of the chain.

@subheading NOTES:

Interrupts are disabled during the prepend to ensure the atomicity of
the operation.

Use @code{@value{DIRPREFIX}chain_prepend_unprotected()} to avoid disabling of
interrupts.
