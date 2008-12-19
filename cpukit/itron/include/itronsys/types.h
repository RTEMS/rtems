/**
 * @file itronsys/types.h
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _ITRONSYS_TYPES_H
#define _ITRONSYS_TYPES_H

#include <rtems/score/object.h>

#ifdef __cplusplus
extern "C" {
#endif

/* XXX some of these may need to be moved */
/*
 *  XXX: These names are too short and conflict with numerous
 *       off the shell programs including paranoia.
 */

typedef int8_t        B;       /* signed 8-bit integer */
typedef int16_t       H;       /* signed 16-bit integer */
typedef int32_t       W;       /* signed 32-bit integer */
typedef uint8_t      UB;      /* unsigned 8-bit integer */
typedef uint16_t     UH;      /* unsigned 16-bit integer */
typedef uint32_t     UW;      /* unsigned 32-bit integer */

typedef uint32_t     VW;      /* unpredictable data type (32-bit size) */
typedef uint16_t     VH;      /* unpredictable data type (16-bit size) */
typedef uint8_t      VB;      /* unpredictable data type (8-bit size) */

typedef void        *VP;      /* pointer to an unpredictable data type */

typedef void (*FP)(void);     /* program start address */

/*
 *  6.4 Data Types
 *
 *  The difference between VB, VH and VW and B, H and W is that only the
 *  number of bits is known for the former, not the data type of the
 *  contents.  The latter clearly represent integers.
 */

/*
 *  Data Types Dependent on ITRON Specification
 *
 *  In order to clarify the meanings of certain parameters, the
 *  following names are used for data types which appear frequently
 *  and/or have special meanings.
 */

typedef int32_t     INT;    /* Signed integer (bit width of processor) */
typedef uint32_t   UINT;   /* Unsigned integer (bit width of processor) */
typedef bool       BOOL;  /* Boolean value.  TRUE (1) or FALSE (0). */
typedef int16_t     FN;     /* Function code. Signed integer. Maximum 2 bytes. */
typedef Objects_Id ID;     /* Object ID number (???id) */
                           /*   Value range depends on the system.  Usually */
                           /*   a signed integer.  Certain ID values may */
                           /*   represent objects on other nodes when the */
                           /*   connection function is supported. */

typedef uint32_t   BOOL_ID;/* Boolean value or ID number */

typedef int32_t   HNO;      /* Handler number */
typedef int32_t   RNO;      /* Rendezvous number */
typedef int32_t   NODE;     /* Node Number.  Usually a signed integer. */
typedef int32_t   ATR;      /* Object or handler attribute. */
                           /*   An unsigned integer. */
typedef int32_t   ER;       /* Error code.  A signed integer. */
typedef uint32_t   PRI;    /* Task priority.  A signed integer. */
typedef int32_t   TMO;      /* Timeout value.  A signed integer. */
                           /*   TMO_POL = 0 indicates polling, */
                           /*   while TMO_FEVR = -1 indicates wait forever. */

/*
 *   6.6 Common Constants and Data Structure Packet Formats
 */

#define NADR   (-1)   /* invalid address or pointer value */

#ifndef TRUE
#define TRUE     1    /* true */
#endif

#ifndef FALSE
#define FALSE    0    /* false */
#endif

/*
 *  ATR tskatr, intatr, cycatr, almatr, svcatr, excatr:
 *
 *  TA_ASM indicates that the task or handler is directly started
 *         at the assembly language level.  The TA_ASM attribute has the
 *         opposite meaning of TA_HLNG.
 *
 *  NOTE: Really in <itronsys/task.h>
 */

#if 0
#define TA_ASM    0x00   /* program written in assembly language */
#define TA_HLNG   0x01   /* program written in high-level language */
#endif

/*
 *  TMO tmout:
 */

#define TMO_POL      0    /* polling */
#define TMO_FEVR   (-1)   /* wait forever */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
