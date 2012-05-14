@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Miscellaneous

@section Fatal Error Default Handler

The @code{_CPU_Fatal_halt} routine is the default fatal error handler. This
routine copies _error into a known place -- typically a stack location or
a register, optionally disables interrupts, and halts/stops the CPU.  It
is prototyped as follows and is often implemented as a macro:

@example
void _CPU_Fatal_halt(
    unsigned32 _error
);
@end example

@section Processor Endianness

Endianness refers to the order in which numeric values are stored in
memory by the microprocessor.  Big endian architectures store the most
significant byte of a multi-byte numeric value in the byte with the lowest
address.  This results in the hexadecimal value 0x12345678 being stored as
0x12345678 with 0x12 in the byte at offset zero, 0x34 in the byte at
offset one, etc..  The Motorola M68K and numerous RISC processor families
is big endian.  Conversely, little endian architectures store the least
significant byte of a multi-byte numeric value in the byte with the lowest
address.  This results in the hexadecimal value 0x12345678 being stored as
0x78563412 with 0x78 in the byte at offset zero, 0x56 in the byte at
offset one, etc..  The Intel ix86 family is little endian.  
Interestingly, some CPU models within the PowerPC and MIPS architectures
can be switched between big and little endian modes.  Most embedded
systems use these families strictly in big endian mode.

RTEMS must be informed of the byte ordering for this microprocessor family
and, optionally, endian conversion routines may be provided as part of the
port.  Conversion between endian formats is often necessary in
multiprocessor environments and sometimes needed when interfacing with
peripheral controllers.

@subsection Specifying Processor Endianness

The @code{CPU_BIG_ENDIAN} and @code{CPU_LITTLE_ENDIAN} are
set to specify the endian
format used by this microprocessor.  These macros should not be set to the
same value.  The following example illustrates how these macros should be
set on a processor family that is big endian.

@example
#define CPU_BIG_ENDIAN                           TRUE
#define CPU_LITTLE_ENDIAN                        FALSE
@end example

The @code{CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK} macro is set to the amount of
stack space above the minimum thread stack space required by the MPCI
Receive Server Thread.  This macro is needed because in a multiprocessor
system the MPCI Receive Server Thread must be able to process all
directives.

@example
#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0
@end example

@subsection Endian Swap Unsigned Integers

The port should provide routines to swap sixteen (@code{CPU_swap_u16}) and
thirty-bit (@code{CPU_swap_u32}) unsigned integers.  These are primarily used in
two areas of RTEMS - multiprocessing support and the network endian swap
routines.  The @code{CPU_swap_u32} routine must be implemented as a static
routine rather than a macro because its address is taken and used
indirectly.  On the other hand, the @code{CPU_swap_u16} routine may be
implemented as a macro.

Some CPUs have special instructions that swap a 32-bit quantity in a
single instruction (e.g. i486).  It is probably best to avoid an "endian
swapping control bit" in the CPU.  One good reason is that interrupts
would probably have to be disabled to insure that an interrupt does not
try to access the same "chunk" with the wrong endian.  Another good reason
is that on some CPUs, the endian bit endianness for ALL fetches -- both
code and data -- so the code will be fetched incorrectly.

The following is an implementation of the @code{CPU_swap_u32} routine that will
work on any CPU.  It operates by breaking the unsigned thirty-two bit
integer into four byte-wide quantities and reassemblying them.

@example
static inline unsigned int CPU_swap_u32(
  unsigned int value
)
@{
  unsigned32 byte1, byte2, byte3, byte4, swapped;
 
  byte4 = (value >> 24) & 0xff;
  byte3 = (value >> 16) & 0xff;
  byte2 = (value >> 8)  & 0xff;
  byte1 =  value        & 0xff;
 
  swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  return( swapped );
@}
@end example

Although the above implementation is portable, it is not particularly
efficient.  So if there is a better way to implement this on a particular
CPU family or model, please do so.  The efficiency of this routine has
significant impact on the efficiency of the multiprocessing support code
in the shared memory driver and in network applications using the ntohl()
family of routines.

Most microprocessor families have rotate instructions which can be used to
greatly improve the @code{CPU_swap_u32} routine.  The most common
way to do this is to:

@example
swap least significant two bytes with 16-bit rotate
swap upper and lower 16-bits
swap most significant two bytes with 16-bit rotate
@end example

Some CPUs have special instructions that swap a 32-bit quantity in a
single instruction (e.g. i486).  It is probably best to avoid an "endian
swapping control bit" in the CPU.  One good reason is that interrupts
would probably have to be disabled to insure that an interrupt does not
try to access the same "chunk" with the wrong endian.  Another good reason
is that on some CPUs, the endian bit endianness for ALL fetches -- both
code and data -- so the code will be fetched incorrectly.

Similarly, here is a portable implementation of the @code{CPU_swap_u16}
routine.  Just as with the @code{CPU_swap_u32} routine, the porter
should provide a better implementation if possible.

@example
#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))
@end example


