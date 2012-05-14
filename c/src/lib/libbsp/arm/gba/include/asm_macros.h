/**
 *  @file  asm_macros.h
 *
 *  This include file contains definitions related to the GBA BSP.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*-----------------------------------------------------------------------------*
 * Macros
 *-----------------------------------------------------------------------------*/
#ifdef __asm__
#define PUBLIC_ARM_FUNCTION(label)   .global label ; \
                                     .type label, function ; \
                                     .arm ; \
                                     label:

#define PUBLIC_THUMB_FUNCTION(label) .global label ; \
                                     .type label, function ; \
                                     .thumb_func ; \
                                     label:

#define STATIC_ARM_FUNCTION(label)   .type label, function ; \
                                     .arm ; \
                                     label:

#define STATIC_THUMB_FUNCTION(label) .type label, function ; \
                                     .thumb_func ; \
                                     label:

#define OBJECT(label)                .global label ; \
                                     .type label, object ; \
                                     label:

#define STATIC_OBJECT(label)         .type label, object ; \
                                     label:

#define WEAK_OBJECT(label)           .weak label ; \
                                     .type label, object ; \
                                     label:

#define LABEL_END(label)             .L##label##_end: ; \
                                     .size label, .L##label##_end - label

#endif
