@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Memory Model

@section Introduction

A processor may support any combination of memory
models ranging from pure physical addressing to complex demand
paged virtual memory systems.  RTEMS supports a flat memory
model which ranges contiguously over the processor's allowable
address space.  RTEMS does not support segmentation or virtual
memory of any kind.  The appropriate memory model for RTEMS
provided by the targeted processor and related characteristics
of that model are described in this chapter.

@section Flat Memory Model

The SPARC architecture supports a flat 32-bit address
space with addresses ranging from 0x00000000 to 0xFFFFFFFF (4
gigabytes).  Each address is represented by a 32-bit value and
is byte addressable.  The address may be used to reference a
single byte, half-word (2-bytes), word (4 bytes), or doubleword
(8 bytes).  Memory accesses within this address space are
performed in big endian fashion by the SPARC.  Memory accesses
which are not properly aligned generate a "memory address not
aligned" trap (type number 7).  The following table lists the
alignment requirements for a variety of data accesses:

@ifset use-ascii
@example
@group
          +--------------+-----------------------+
          |   Data Type  | Alignment Requirement |
          +--------------+-----------------------+
          |     byte     |          1            |
          |   half-word  |          2            |
          |     word     |          4            |
          |  doubleword  |          8            |
          +--------------+-----------------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\vrule\strut#&
\hbox to 1.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.75in{\enskip\hfil#\hfil}&
\vrule#\cr
\noalign{\hrule}
&\bf Data Type &&\bf Alignment Requirement&\cr\noalign{\hrule}
&byte&&1&\cr\noalign{\hrule}
&half-word&&2&\cr\noalign{\hrule}
&word&&4&\cr\noalign{\hrule}
&doubleword&&8&\cr\noalign{\hrule}
}}\hfil}
@end tex
@end ifset
 
@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="60%" BORDER=2>
<TR><TD ALIGN=center><STRONG>Data Type</STRONG></TD>
    <TD ALIGN=center><STRONG>Alignment Requirement</STRONG></TD></TR>
<TR><TD ALIGN=center>byte</TD>
    <TD ALIGN=center>1</TD></TR>
<TR><TD ALIGN=center>half-word</TD>
    <TD ALIGN=center>2</TD></TR>
<TR><TD ALIGN=center>word</TD>
    <TD ALIGN=center>4</TD></TR>
<TR><TD ALIGN=center>doubleword</TD>
    <TD ALIGN=center>8</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

Doubleword load and store operations must use a pair
of registers as their source or destination.  This pair of
registers must be an adjacent pair of registers with the first
of the pair being even numbered.  For example, a valid
destination for a doubleword load might be input registers 0 and
1 (i0 and i1).  The pair i1 and i2 would be invalid.  [NOTE:
Some assemblers for the SPARC do not generate an error if an odd
numbered register is specified as the beginning register of the
pair.  In this case, the assembler assumes that what the
programmer meant was to use the even-odd pair which ends at the
specified register.  This may or may not have been a correct
assumption.]

RTEMS does not support any SPARC Memory Management
Units, therefore, virtual memory or segmentation systems
involving the SPARC are not supported.

