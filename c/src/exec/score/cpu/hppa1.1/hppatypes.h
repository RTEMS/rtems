/* hppatypes.h
 *
 *  This include file contains type definitions pertaining to the Hewlett
 *  Packard PA-RISC processor family.
 *
 *  $Id$
 */

#ifndef _INCLUDE_HPPATYPES_H
#define _INCLUDE_HPPATYPES_H

#ifndef ASM
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*
 *  This section defines the basic types for this processor.
 */

typedef unsigned char      unsigned8;  /* 8-bit  unsigned integer */
typedef unsigned short     unsigned16; /* 16-bit unsigned integer */
typedef unsigned int       unsigned32; /* 32-bit unsigned integer */
typedef unsigned long long unsigned64; /* 64-bit unsigned integer */

typedef unsigned16 Priority_Bit_map_control;

typedef char      signed8;      /* 8-bit  signed integer */
typedef short     signed16;     /* 16-bit signed integer */
typedef int       signed32;     /* 32-bit signed integer */
typedef long long signed64;     /* 64 bit signed integer */

typedef unsigned32 boolean;     /* Boolean value   */

typedef float     single_precision;     /* single precision float */
typedef double    double_precision;     /* double precision float */

#ifdef __cplusplus
}
#endif
 
#endif  /* !ASM */
 
#endif /* _INCLUDE_HPPATYPES_H */
/* end of include file */
